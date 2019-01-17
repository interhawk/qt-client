/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "docAttach.h"

#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QString>
#include <QUiLoader>
#include <QUrl>
#include <QVariant>

#include "documents.h"
#include "errorReporter.h"
#include "../common/shortcuts.h"
#include "imageview.h"

#define DEBUG false

class StackDescriptor
{
  public:
    StackDescriptor(QWidget *page, QWidget *doc)
      : stackPage(page),
        docWidget(doc)
    {
    }

    QWidget *stackPage;
    QWidget *docWidget;
};

class docAttachPrivate {
  public:
    docAttachPrivate(docAttach *parent)
      : p(parent)
    {
      // url and file match _docType->populate below, at least for now
      map.insert(-4,                           new StackDescriptor(p->_existingFilePage, p->_exfile));
      map.insert(-3,                           new StackDescriptor(p->_urlPage,     p->_url));
      map.insert(-2,                           new StackDescriptor(p->_filePage,    p->_file));
      map.insert(Documents::Contact,           new StackDescriptor(p->_cntctPage,   p->_cntct));
      map.insert(Documents::CRMAccount,        new StackDescriptor(p->_crmacctPage, p->_crmacct));
      map.insert(Documents::Customer,          new StackDescriptor(p->_custPage,    p->_cust));
      map.insert(Documents::Employee,          new StackDescriptor(p->_empPage,     p->_emp));
      map.insert(Documents::Uninitialized,     new StackDescriptor(p->_filePage,    p->_file));
      map.insert(Documents::Uninitialized,     new StackDescriptor(p->_imagePage,   p->_img));
      map.insert(Documents::Incident,          new StackDescriptor(p->_incdtPage,   p->_incdt));
      map.insert(Documents::Invoice,           new StackDescriptor(p->_invoicePage, p->_invoice));
      map.insert(Documents::Item,              new StackDescriptor(p->_itemPage,    p->_item));
      map.insert(Documents::Opportunity,       new StackDescriptor(p->_oppPage,     p->_opp));
      map.insert(Documents::Project,           new StackDescriptor(p->_projPage,    p->_proj));
      map.insert(Documents::PurchaseOrder,     new StackDescriptor(p->_poPage,      p->_po));
      map.insert(Documents::Quote,             new StackDescriptor(p->_quPage,      p->_qu));
      map.insert(Documents::ReturnAuth,        new StackDescriptor(p->_raPage,      p->_ra));
      map.insert(Documents::SalesOrder,        new StackDescriptor(p->_soPage,      p->_so));
      map.insert(Documents::Vendor,            new StackDescriptor(p->_vendPage,    p->_vend));
      map.insert(Documents::Uninitialized,     new StackDescriptor(p->_urlPage,     p->_url));
      map.insert(Documents::WorkOrder,         new StackDescriptor(p->_woPage,      p->_wo));

      if (! _x_privileges) return; // Qt Designer doesn't connect to the database

      XSqlQuery q("SELECT * FROM source"
                  " WHERE source_widget NOT IN ('', 'core');");
      QUiLoader uil(p);
      while (q.next())
      {
        QWidget *w = 0;
        QString  description = q.value("source_widget").toString();
        if (DEBUG)
          qDebug() << "checking" << q.value("source_name") << description;
        if (description.startsWith("SELECT", Qt::CaseInsensitive))
        {
          XComboBox *c = new XComboBox();
          c->populate(description);
          w = c;
        }
        else if (description.contains("Cluster"))
        {
          w = uil.createWidget(description, p,
                               "_" + q.value("source_name").toString());
        }
        if (w) {
          QString litValue = q.value("source_descrip").toString();
          QWidget     *page = new QWidget();
          QFormLayout *lyt  = new QFormLayout(p);
          QLabel      *lit  = new QLabel(QT_TRANSLATE_NOOP("docAttach", litValue));
          page->setLayout(lyt);
          lyt->addRow(lit, w);
          p->_documentsStack->addWidget(page);
          map.insert(q.value("source_docass_num").toInt(), new StackDescriptor(page, w));
          if (DEBUG) qDebug() << "created a widget for" << description;
        }
        else
        {
          qDebug() << "Could not create a widget for" << description;
        }
      }
      ErrorReporter::error(QtCriticalMsg, 0, "Error Getting Document Types",
                           q, __FILE__, __LINE__);
    }

    docAttach *p;
    QMap<int, StackDescriptor*> map;
};

/**
 *  Constructs a docAttach as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 *
 *  This screen will receive the source type and id from the calling screen.
 *  Then the user will select a target type and id on this screen.
 *  When the user clicks Save, a new row will be entered into docass and
 *  the screen will return a docass_id to the calling screen.
 */

docAttach::docAttach(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
  : QDialog(parent, fl), ScriptableWidget(this)
{
  setupUi(this);
  setObjectName(name ? name : "docAttach");
  setModal(modal);

  _p = new docAttachPrivate(this);

  _save = _buttonBox->button(QDialogButtonBox::Save);
  _save->setEnabled(false);
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(_docType,   SIGNAL(newID(int)), this, SLOT(sHandleButtons()));
  connect(_filetitle,   SIGNAL(textChanged(QString)), this, SLOT(sHandleButtons()));
  connect(_fileList,  SIGNAL(clicked()),  this, SLOT(sFileList()));
  connect(_save,      SIGNAL(clicked()),  this, SLOT(sSave()));
  connect(_fileAdd,   SIGNAL(clicked()),  this, SLOT(sAddFilePriv()));
  connect(_fileRem,   SIGNAL(clicked()),  this, SLOT(sRemFilePriv()));
  connect(_urlAdd,    SIGNAL(clicked()),  this, SLOT(sAddUrlPriv()));
  connect(_urlRem,    SIGNAL(clicked()),  this, SLOT(sRemUrlPriv()));

  _sourcetype = "";
  _sourceid = -1;
  _targetid = -1;
  _id = -1;
  _mode = "new";

  _po->setAllowedTypes(OrderLineEdit::Purchase);
  _so->setAllowedTypes(OrderLineEdit::Sales);
  _ra->setAllowedTypes(OrderLineEdit::Return);

  _docType->populate("SELECT * FROM"
                     "(SELECT source_docass_num, source_descrip, source_name"
                     "  FROM source"
                     " WHERE source_widget != ''"
                     " UNION SELECT -2, 'File',     'FILE'"
                     " UNION SELECT -3, 'Web Site', 'URL' "
                     " UNION SELECT -4, 'Existing Document', 'XFILE') data"
                     " ORDER BY source_descrip;");

  _charassFile->setType("FILE");
  _charassUrl->setType("FILE");
  _charid = qrand();
  _charassFile->setId(_charid);
  _charassUrl->setId(_charid);

  _fileAvailable->addColumn(tr("Available Roles"),    -1,  Qt::AlignLeft, true, "file_available" );
  _fileAssigned->addColumn(tr("Assigned Roles"),  -1,  Qt::AlignLeft, true, "file_assigned" );
  _urlAvailable->addColumn(tr("Available Roles"),     -1,  Qt::AlignLeft, true, "url_available" );
  _urlAssigned->addColumn(tr("Assigned Roles"),   -1,  Qt::AlignLeft, true, "url_assigned" );

#ifndef Q_OS_MAC
    _fileList->setMaximumWidth(25);
#else
    _fileList->setMinimumWidth(60);
    _fileList->setMinimumHeight(32);
#endif

    shortcuts::setStandardKeys(this);
    adjustSize();
}

docAttach::~docAttach()
{
  // no need to delete child widgets, Qt does it all for us
}

int docAttach::id()
{
  return _id;
}

QString docAttach::mode()
{
  return _mode;
}

QString docAttach::purpose()
{
  return _purpose;
}

int docAttach::sourceId()
{
  return _sourceid;
}

QString docAttach::sourceType()
{
  return _sourcetype;
}

int docAttach::targetId()
{
  return _targetid;
}

QString docAttach::targetType()
{
  return _targettype;
}

void docAttach::languageChange()
{
  retranslateUi(this);
}

void docAttach::set(const ParameterList &pParams)
{
  QVariant param;
  bool     valid;

  //source type from document widget
  param = pParams.value("sourceType", &valid);
  if (valid)
    _sourcetype = param.toString();

  //source id from document widget
  param = pParams.value("source_id", &valid);
  if (valid)
    _sourceid = param.toInt();

  // Only urls are editable
  param = pParams.value("url_id", &valid);
  if(valid)
  {
    if (DEBUG) qDebug() << "got url_id" << param;
    XSqlQuery qry;
    _id = param.toInt();
    qry.prepare("SELECT url_source, url_source_id, url_title, url_url, url_stream, url_mime_type, "
                "       docass_target_id, docass_notes "
                "  FROM url"
                "  JOIN docass ON url_id = docass_id"
                " WHERE (url_id=:url_id);" );
    qry.bindValue(":url_id", _id);
    qry.exec();
    if(qry.first())
    {
      setWindowTitle(tr("Edit Attachment Link"));
      _targetid = qry.value("docass_target_id").toInt();
      _charid = _targetid;
      _charassFile->setId(_charid);
      _charassUrl->setId(_charid);
      QUrl url(qry.value("url_url").toString());
      if (url.scheme().isEmpty())
        url.setScheme("file");

      if (DEBUG) qDebug() << qry.value("url_url") << "converted to" << url;

      _url->setText(url.toString());
      if (url.scheme() == "file")
      {
        if (DEBUG)
          qDebug() << "file title:"    << qry.value("url_title").toString()
                   << " text:"         << url.toString()
                   << "stream length:" << qry.value("url_stream").toString().length();
        _docType->setId(-2);
        _filetitle->setText(qry.value("url_title").toString());
        _file->setText(url.toString());
        _mimeType->setText(qry.value("url_mime_type").toString());
        if (qry.value("url_stream").toString().length())
        {
          _fileList->setEnabled(false);
          _file->setEnabled(false);
          _saveDbCheck->setEnabled(false);
        }
      }
      else
      {
        if (DEBUG)
          qDebug() << "! file title:" << qry.value("url_title").toString()
                   << " text:" << url.toString();
        _docType->setId(-3);
        _urltitle->setText(qry.value("url_title").toString());
        _url->setText(url.toString());
      }
       _notes->setText(qry.value("docass_notes").toString());
      _mode = "edit";
      _docType->setEnabled(false);
    }
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error URL"),
                         qry, __FILE__, __LINE__);
  }

  sPopulateDocPrivs();
}

void docAttach::showEvent(QShowEvent *e)
{
  loadScriptEngine();
  QWidget::showEvent(e);
}

void docAttach::sHandleNewId(int id)
{
  _id = id;
  _save->setEnabled(id != -1);
}

void docAttach::sHandleButtons()
{
  if (_docType->id() == -1) {
    return;
  }
  _docAttachPurpose->setEnabled(true);

  StackDescriptor*pageDesc   = _p->map.value(_docType->id());
  QWidget        *pageWidget = qobject_cast<QWidget *>(pageDesc->stackPage);
  VirtualCluster *docCluster = qobject_cast<VirtualCluster *>(pageDesc->docWidget);
  XComboBox      *docCombo   = qobject_cast<XComboBox *>(pageDesc->docWidget);
  WoCluster      *wocluster  = qobject_cast<WoCluster *>(pageDesc->docWidget);

  if (! pageWidget)
  {
    _save->setEnabled(false);
    return;
  }
  _documentsStack->setCurrentWidget(pageWidget);

  if (docCluster)
  {
    connect(docCluster, SIGNAL(valid(bool)), _save, SLOT(setEnabled(bool)));
    connect(docCluster, SIGNAL(newId(int)),   this, SLOT(sHandleNewId(int)));
    _save->setEnabled(docCluster->isValid());
  }
  else if (docCombo)
  {
    connect(docCombo, SIGNAL(valid(bool)), _save, SLOT(setEnabled(bool)));
    _save->setEnabled(docCombo->isValid());
  }
  else if (wocluster)
  {
    connect(wocluster, SIGNAL(valid(bool)), _save, SLOT(setEnabled(bool)));
    connect(wocluster, SIGNAL(newId(int)),   this, SLOT(sHandleNewId(int)));
    _save->setEnabled(wocluster->isValid());
  }
  else
  {
    if (DEBUG)
      qDebug() << pageDesc->docWidget->objectName() << "is a"
               << pageDesc->docWidget->metaObject()->className();
    _docAttachPurpose->setEnabled(false);
    _docAttachPurpose->setCurrentIndex(0);
    _save->setEnabled(true); // presumably we're on the file or url stack page
  }
}

void docAttach::sSave()
{  
  if (_docType->id() == -1) {
    return;
  }

  _docAttachPurpose->setEnabled(true);
  XSqlQuery newDocass;
  QString title;
  QUrl url;
  QStringList isFile;
  isFile << "URL" << "FILE";

  //set the purpose
  if (_docAttachPurpose->currentIndex() == 0)
    _purpose = "S";
  else if (_docAttachPurpose->currentIndex() == 1)
    _purpose = "A";
  else if (_docAttachPurpose->currentIndex() == 2)
    _purpose = "C";
  else if (_docAttachPurpose->currentIndex() == 3)
    _purpose = "D";

  StackDescriptor*pageDesc   = _p->map.value(_docType->id());
  VirtualCluster *docCluster = qobject_cast<VirtualCluster *>(pageDesc->docWidget);
  XComboBox      *docCombo   = qobject_cast<XComboBox *>(pageDesc->docWidget);
  WoCluster      *wocluster  = qobject_cast<WoCluster *>(pageDesc->docWidget);

  _targettype = _docType->code();
  if (docCluster)
  {
    _targetid = docCluster->id();
  }
  else if (docCombo)
  {
    _targetid = docCombo->id();
  }
  else if (wocluster)
  {
    _targetid = wocluster->id();
  }
  else if (_documentsStack->currentWidget() == _filePage)
  {
    if(_file->text().trimmed().isEmpty())
    {
      QMessageBox::warning( this, tr("Must Specify file"),
                            tr("You must specify a file before you may save.") );
      return;
    }

     _targettype = _saveDbCheck->isChecked() ? "FILE" : "URL";
     title = _filetitle->text();
     url = QUrl(_file->text());
     if (url.scheme().isEmpty())
       url.setScheme("file");
  }
  else if (_documentsStack->currentWidget() == _urlPage)
  {
    if(_url->text().trimmed().isEmpty())
    {
      QMessageBox::warning( this, tr("Must Specify file"),
                            tr("You must specify a file before you may save.") );
      return;
    }

    _targettype = "URL";
    title = _urltitle->text();
    url = QUrl(_url->text());
    if (url.scheme().isEmpty())
      url.setScheme("http");
  }

  XSqlQuery rollback;
  rollback.prepare("ROLLBACK;");

  emit saveBeforeBegin();
  if (_saveStatus==Failed)
    return;

  XSqlQuery begin("BEGIN;");

  emit saveAfterBegin();
  if (_saveStatus==Failed)
  {
    rollback.exec();
    return;
  }

  if (_targettype == "IMG")
  {
    // First determine if the id is in the image table, and not one of it's inherited versions
    // if it is not then we will create a copy in the image table to keep the FK's working
    XSqlQuery qq;
    qq.prepare("SELECT image_id FROM ONLY image WHERE image_id=:image_id");
    qq.bindValue(":image_id", _targetid);
    qq.exec();
    if(!qq.first())
    {
      if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error saving"),
                               qq, __FILE__, __LINE__))
      {
        emit saveBeforeRollback(&qq);
        rollback.exec();
        emit saveAfterRollback(&qq);
        return;
      }

      qq.exec("SELECT nextval(('\"image_image_id_seq\"'::text)::regclass) AS newid;");
      if(qq.first())
      {
        int newid = qq.value("newid").toInt();
        qq.prepare("INSERT INTO image (image_id, image_name, image_descrip, image_data) "
                   "SELECT :newid, image_name, image_descrip, image_data"
                   "  FROM image WHERE image_id=:image_id;");
        qq.bindValue(":newid", newid);
        qq.bindValue(":image_id", _targetid);
        qq.exec();
        if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error saving"),
                                 qq, __FILE__, __LINE__))
        {
          emit saveBeforeRollback(&qq);
          rollback.exec();
          emit saveAfterRollback(&qq);
          return;
        }
        _targetid = newid;
      }
      else if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error saving"),
                               qq, __FILE__, __LINE__))
      {
        emit saveBeforeRollback(&qq);
        rollback.exec();
        emit saveAfterRollback(&qq);
        return;
      }
    }
     // For now images are handled differently because of legacy structures...
    newDocass.prepare( "INSERT INTO imageass "
                       "( imageass_source, imageass_source_id, imageass_image_id, imageass_purpose ) "
                       "VALUES "
                       "( :docass_source_type, :docass_source_id, :docass_target_id, :docass_purpose )"
                       " RETURNING imageass_id AS docass_id, imageass_image_id AS docass_target_id;" );
  }
  else if (isFile.contains(_targettype))
  {
    if(!url.isValid())
    {
      QMessageBox::warning( this, tr("Must Specify valid path"),
                            tr("You must specify a path before you may save.") );
      emit saveBeforeRollback(new XSqlQuery());
      rollback.exec();
      emit saveAfterRollback(new XSqlQuery());
      return;
    }

    QByteArray  bytarr;
    QFileInfo fi(url.toLocalFile());

    if(_saveDbCheck->isChecked() &&
       (url.scheme()=="file") &&
       (_mode == "new"))
    {
      if (!fi.exists())
      {
        QMessageBox::warning( this, tr("File Error"),
                             tr("File %1 was not found and will not be saved.").arg(url.toLocalFile()));
        emit saveBeforeRollback(new XSqlQuery());
        rollback.exec();
        emit saveAfterRollback(new XSqlQuery());
        return;
      }
      QFile sourceFile(url.toLocalFile());
      if (!sourceFile.open(QIODevice::ReadOnly))
      {
        QMessageBox::warning( this, tr("File Open Error"),
                             tr("Could not open source file %1 for read.")
                                .arg(url.toLocalFile()));
        emit saveBeforeRollback(new XSqlQuery());
        rollback.exec();
        emit saveAfterRollback(new XSqlQuery());
        return;
      }
      bytarr = sourceFile.readAll();
      url.setUrl(fi.fileName().remove(" "));
      url.setScheme("");
    }

    if (_mode == "new" && bytarr.isNull())
    {
      newDocass.prepare( "INSERT INTO docass ("
                         "  docass_source_id, docass_source_type,"
                         "  docass_target_id, docass_target_type,"
                         "  docass_purpose, docass_notes"
                         ") VALUES ("
                         "  :docass_source_id, :docass_source_type,"
                         "  createurl(:title, :url), 'URL'::text,"
                         "  'S'::bpchar, :docass_notes"
                         ") RETURNING docass_id, docass_target_id;");
    }
    else if (_mode == "new")
    {
      newDocass.prepare( "INSERT INTO docass ("
                         "  docass_source_id, docass_source_type,"
                         "  docass_target_id, docass_target_type,"
                         "  docass_purpose, docass_notes"
                         ") VALUES ("
                         "  :docass_source_id, :docass_source_type,"
                         "  createfile(:title, :url, :stream, :mime_type), 'FILE'::text,"
                         "  'S'::bpchar, :docass_notes"
                         ") RETURNING docass_id, docass_target_id;");

      QMimeDatabase mimeDb;
      QMimeType mime = mimeDb.mimeTypeForFileNameAndData(_filetitle->text(), bytarr);

      newDocass.bindValue(":stream", bytarr);
      newDocass.bindValue(":mime_type", mime.name());
    }
    else
    {
      newDocass.prepare( "UPDATE url SET"
                         "  url_title = :title,"
                         "  url_url = :url,"
                         "  url_notes = :docass_notes "
                         " WHERE url_id=:url_id;");
    }

    newDocass.bindValue(":url_id", _id);
    newDocass.bindValue(":title", title);
    newDocass.bindValue(":url", url.toString());
  }
  else
  {
    newDocass.prepare( "INSERT INTO docass "
                       "( docass_source_type, docass_source_id, docass_target_type, "
                       "  docass_target_id, docass_purpose, docass_notes ) "
                       "VALUES "
                       "( :docass_source_type, :docass_source_id, :docass_target_type, "
                       "  :docass_target_id, :docass_purpose, :docass_notes )"
                       " RETURNING docass_id, docass_target_id;");
    newDocass.bindValue(":docass_target_type", _targettype);
  }

  if (_targettype == _sourcetype &&
      _targetid == _sourceid)
  {
    QMessageBox::critical(this,tr("Invalid Selection"),
                          tr("You may not attach a document to itself."));
    emit saveBeforeRollback(new XSqlQuery());
    rollback.exec();
    emit saveAfterRollback(new XSqlQuery());
    return;
  }

  newDocass.bindValue(":docass_source_type", _sourcetype);
  newDocass.bindValue(":docass_source_id", _sourceid);
  newDocass.bindValue(":docass_target_id", _targetid);
  newDocass.bindValue(":docass_purpose", _purpose);
  newDocass.bindValue(":docass_notes", _notes->text().trimmed());

  newDocass.exec();

  if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error saving"),
                           newDocass, __FILE__, __LINE__))
  {
    emit saveBeforeRollback(&newDocass);
    rollback.exec();
    emit saveAfterRollback(&newDocass);
    return;
  }

  if (newDocass.first() &&
      newDocass.value("docass_id").toInt() > 0 &&
      newDocass.value("docass_target_id").toInt() > 0)
  {
    _id = newDocass.value("docass_id").toInt();
    _targetid = newDocass.value("docass_target_id").toInt();

    // Update temporary Characteristics & Permissions with actual file ID
    if (isFile.contains(_targettype) && _targetid != _charid)
    {
      XSqlQuery updc;
      updc.prepare("UPDATE charass SET charass_target_id=:targetid "
                   " WHERE charass_target_type = 'FILE' "
                   "   AND charass_target_id=:charid; ");
      updc.bindValue(":targetid", _targetid);  
      updc.bindValue(":charid", _charid);
      updc.exec();
      if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error saving"),
                               updc, __FILE__, __LINE__))
        return;

      updc.prepare("UPDATE filegrp SET filegrp_file_id=:targetid "
                   " WHERE filegrp_file_id = :charid; ");
      updc.bindValue(":targetid", _targetid);  
      updc.bindValue(":charid", _charid);
      updc.exec();
      if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error saving"),
                               updc, __FILE__, __LINE__))
        return;

      _charid = _targetid;
      _charassFile->setId(_charid);
      _charassUrl->setId(_charid);
    }
  }

  emit saveBeforeCommit();

  if (_saveStatus==Failed)
  {
    rollback.exec();
    return;
  }

  XSqlQuery commit("COMMIT;");

  bool tryAgain = false;

  do
  {
    emit saveAfterCommit();
    if (_saveStatus==Failed)
    {
      QMessageBox failure(QMessageBox::Critical, tr("Script Error"),
                          tr("A script has failed after the main window saved successfully. How do "
                             "you wish to proceed?"));
      QPushButton* retry = failure.addButton(tr("Retry"), QMessageBox::NoRole);
      failure.addButton(tr("Ignore"), QMessageBox::YesRole);
      QPushButton* cancel = failure.addButton(QMessageBox::Cancel);
      failure.setDefaultButton(cancel);
      failure.setEscapeButton((QAbstractButton*)cancel);
      failure.exec();
      if (failure.clickedButton() == (QAbstractButton*)retry)
      {
        setSaveStatus(OK);
        tryAgain = true;
      }
      else if (failure.clickedButton() == (QAbstractButton*)cancel)
        return;
    }
  }
  while (tryAgain);

  accept();
  return;
}

void docAttach::sFileList()
{
  _file->setText(QString("file:%1").arg(QFileDialog::getOpenFileName( this, tr("Select File"), QString::null)));
  if (!_filetitle->text().length())
  {
    QFileInfo fi = QFileInfo(_file->text());
    _filetitle->setText(fi.fileName());
  }
}

void docAttach::sPopulateDocPrivs()
{
   XSqlQuery privs;

   privs.prepare("SELECT grp_id, grp_name AS file_available "
                 " FROM grp "
                 " WHERE grp_id NOT IN (SELECT filegrp_grp_id FROM filegrp WHERE filegrp_file_id=:file);" );
   privs.bindValue(":file", _charid);
   privs.exec();
   if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Retrieving Permissions"),
                            privs, __FILE__, __LINE__))
     return;
   else
     _fileAvailable->populate(privs);

   privs.prepare("SELECT grp_id, grp_name AS file_assigned "
                 " FROM filegrp "
                 " JOIN grp ON filegrp_grp_id=grp_id AND filegrp_file_id=:file;" );
   privs.bindValue(":file", _charid);
   privs.exec();
   if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Retrieving Permissions"),
                            privs, __FILE__, __LINE__))
     return;
   else
     _fileAssigned->populate(privs);

   privs.prepare("SELECT grp_id, grp_name AS url_available "
                 " FROM grp "
                 " WHERE grp_id NOT IN (SELECT filegrp_grp_id FROM filegrp WHERE filegrp_file_id=:url);" );
   privs.bindValue(":url", _charid);
   privs.exec();
   if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Retrieving Permissions"),
                            privs, __FILE__, __LINE__))
     return;
   else
     _urlAvailable->populate(privs);

   privs.prepare("SELECT grp_id, grp_name AS url_assigned "
                 " FROM filegrp "
                 " JOIN grp ON filegrp_grp_id=grp_id AND filegrp_file_id=:url;" );
   privs.bindValue(":url", _charid);
   privs.exec();
   if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Retrieving Permissions"),
                                 privs, __FILE__, __LINE__))
     return;
   else
     _urlAssigned->populate(privs);

}

void docAttach::sAddFilePriv()
{
  XSqlQuery addq;

  if (_fileAvailable->id() == -1)
    return;

  addq.prepare("INSERT INTO filegrp (filegrp_file_id, filegrp_grp_id) "
               " VALUES (:file, :role) "
               " ON CONFLICT DO NOTHING;" );
  addq.bindValue(":role", _fileAvailable->id());
  addq.bindValue(":file", _charid);
  addq.exec();
  if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Adding Permissions"),
                                 addq, __FILE__, __LINE__))
     return;

  sPopulateDocPrivs();
}

void docAttach::sRemFilePriv()
{
  XSqlQuery remq;

  if (_fileAssigned->id() == -1)
    return;

  remq.prepare("DELETE FROM filegrp "
               " WHERE filegrp_file_id = :file "
               "   AND filegrp_grp_id = :role " );
  remq.bindValue(":role", _fileAssigned->id());
  remq.bindValue(":file", _charid);
  remq.exec();
  if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Removing Permissions"),
                                 remq, __FILE__, __LINE__))
     return;

  sPopulateDocPrivs();
}

void docAttach::sAddUrlPriv()
{
  XSqlQuery addq;

  if (_urlAvailable->id() == -1)
    return;

  addq.prepare("INSERT INTO filegrp (filegrp_file_id, filegrp_grp_id) "
               " VALUES (:url, :role) "
               " ON CONFLICT DO NOTHING;" );
  addq.bindValue(":role", _urlAvailable->id());
  addq.bindValue(":url", _charid);
  addq.exec();
  if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Adding Permissions"),
                                 addq, __FILE__, __LINE__))
     return;

  sPopulateDocPrivs();
}

void docAttach::sRemUrlPriv()
{
  XSqlQuery remq;

  if (_urlAssigned->id() == -1)
    return;

  remq.prepare("DELETE FROM filegrp "
               " WHERE filegrp_file_id = :url "
               "   AND filegrp_grp_id = :role " );
  remq.bindValue(":role", _urlAssigned->id());
  remq.bindValue(":url", _charid);
  remq.exec();
  if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error Removing Permissions"),
                                 remq, __FILE__, __LINE__))
     return;

  sPopulateDocPrivs();
}
