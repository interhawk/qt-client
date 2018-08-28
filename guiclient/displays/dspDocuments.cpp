/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "dspDocuments.h"
#include "docAttach.h"

#include <QDesktopServices>
#include <QDialog>
#include <QMenu>
#include <QMessageBox>
#include <QVariant>
#include <QUrl>

#include "errorReporter.h"
#include "scriptablewidget.h"
#include "parameterwidget.h"

dspDocuments::dspDocuments(QWidget* parent, const char*, Qt::WindowFlags fl)
  : display(parent, "dspDocuments", fl)
{
  setWindowTitle(tr("Documents"));
  setReportName("Documents");
  setMetaSQLOptions("documents", "detail");
  setParameterWidgetVisible(true);
  setSearchVisible(true);
  setUseAltId(true);

  QString lnkSql = QString("SELECT source_name, source_descrip "
                           "FROM source "
                           "WHERE source_widget != '' "
                           "ORDER BY source_descrip;");

  parameterWidget()->append(tr("Assigned To"), "assignedTo", ParameterWidget::Multiselect,  QVariant(), false, lnkSql);
  parameterWidget()->append(tr("Created Start Date"), "importDateStart", ParameterWidget::Date);
  parameterWidget()->append(tr("Created End Date"), "importDateEnd", ParameterWidget::Date);
  parameterWidget()->append(tr("Created By"),   "owner",   ParameterWidget::User);

  list()->addColumn(tr("Type"),              -1,    Qt::AlignLeft,   true,  "doctype" );
  list()->addColumn(tr("Number"),            -1,    Qt::AlignLeft,   true,  "altid" );
  list()->addColumn(tr("Title"),             -1,    Qt::AlignLeft,   true,  "title" );
  list()->addColumn(tr("Description/Notes"), -1,    Qt::AlignLeft,   true,  "descrip" );
  list()->addColumn(tr("Created By"),        -1,    Qt::AlignLeft,   true,  "username" );
  list()->addColumn(tr("Created"),           -1,    Qt::AlignLeft,   true,  "assigned" );
  list()->addColumn(tr("Can View"),   _ynColumn,  Qt::AlignCenter,  false,  "canview");
  list()->addColumn(tr("Can Edit"),   _ynColumn,  Qt::AlignCenter,  false,  "canedit");

  setupCharacteristics("FILE");
}

enum SetResponse dspDocuments::set(const ParameterList &pParams)
{
  XWidget::set(pParams);
  QVariant param;
  bool     valid;

  parameterWidget()->setSavedFilters();

  param = pParams.value("file_id", &valid);
  if (valid)
    parameterWidget()->setDefault(tr("Customer"), param.toInt());

  return NoError;
}

void dspDocuments::sPopulateMenu(QMenu *pMenu, QTreeWidgetItem *pSelected, int pColumn)
{
  Q_UNUSED(pColumn);

  QAction* viewDocAct = pMenu->addAction(tr("Open Document"), this, SLOT(sViewDoc()));
  viewDocAct->setEnabled(_privileges->check("ViewDocuments"));

  QAction* editDocAct = pMenu->addAction(tr("Edit Document"), this, SLOT(sEditDoc()));
  editDocAct->setEnabled(_privileges->check("ViewDocuments"));

  if (pSelected->parent())
  {
    pMenu->addSeparator();

    QAction* editDocAct = pMenu->addAction(tr("Open Assignment"), this, SLOT(sOpenAssignment()));
    editDocAct->setEnabled(list()->currentItem()->rawValue("canedit").toBool() || 
                           list()->currentItem()->rawValue("canview").toBool());

    QAction* detachDocAct = pMenu->addAction(tr("Detach Assignment"), this, SLOT(sDetach()));
    detachDocAct->setEnabled(_privileges->check("ViewDocuments"));
  }

}

void dspDocuments::sEditDoc()
{
  sOpenDoc("edit");
}

void dspDocuments::sViewDoc()
{
  sOpenDoc("view");
}

void dspDocuments::sOpenDoc(QString mode)
{
  QString ui;
  QString docType = list()->currentItem()->rawValue("doctype").toString();
  int targetid = list()->id();
  ParameterList params;
  params.append("mode", mode);

  if (mode == "edit")
  {
    params.append("url_id", targetid);

    docAttach newdlg(this, "", true);
    newdlg.set(params);
    newdlg.exec();

    return;
  }

  XSqlQuery qfile;
  qfile.prepare("SELECT url_id, url_source_id, url_source, url_title, url_url, url_stream"
                " FROM url"
                " WHERE (url_id=:url_id);");

  qfile.bindValue(":url_id", list()->id());
  qfile.exec();

  // If file is in the database, copy to a temp. directory in the file system and open it.
  if (qfile.first() && (docType == "FILE"))
  {
    QFileInfo fi( qfile.value("url_url").toString() );
    QDir tdir;

#ifdef Q_OS_WIN
    QString fileName = fi.fileName().remove(" ");
#else
    QString fileName = fi.fileName();
#endif
    QString filePath = tdir.tempPath() + "/xtTempDoc/" +
                       qfile.value("url_id").toString() + "/";
    QFile tfile(filePath + fileName);

    // Remove any previous watches
    ScriptableWidget::_guiClientInterface->removeDocumentWatch(tfile.fileName());

    if (! tdir.exists(filePath))
      tdir.mkpath(filePath);

    if (!tfile.open(QIODevice::WriteOnly))
    {
      QMessageBox::warning( this, tr("File Open Error"),
                            tr("Could Not Create File %1.").arg(tfile.fileName()) );
      return;
    }
    tfile.write(qfile.value("url_stream").toByteArray());
    QUrl urldb;
    urldb.setUrl(tfile.fileName());
#ifndef Q_OS_WIN
    urldb.setScheme("file");
#endif
    tfile.close();
    if (! QDesktopServices::openUrl(urldb))
    {
      QMessageBox::warning(this, tr("File Open Error"),
                           tr("Could not open %1.").arg(urldb.toString()));
      return;
    }

    // Add a watch to the file that will save any changes made to the file back to the database.
    ScriptableWidget::_guiClientInterface->addDocumentWatch(tfile.fileName(),qfile.value("url_id").toInt());
    return;
  }
  else if (ErrorReporter::error(QtCriticalMsg, this,
                                tr("Error Getting Assignment"),
                                qfile, __FILE__, __LINE__))
    return;
  else
  {
    QUrl url(qfile.value("url_url").toString());
    if (url.scheme().isEmpty())
      url.setScheme("file");
    QDesktopServices::openUrl(url);
    return;
  }
}

void dspDocuments::sOpenAssignment()
{
  ParameterList params;
  XSqlQuery doc;
  QString ui;

  if (list()->currentItem()->rawValue("canedit").toBool())
    params.append("mode", "edit");
  else
    params.append("mode", "view");

  doc.prepare("SELECT source_key_field, source_uiform_name, docass_source_id "
              "  FROM source "
              "  JOIN docass on source_name = docass_source_type "
              " WHERE docass_id = :docid;" );
  doc.bindValue(":docid", list()->id());
  doc.exec();
  if (ErrorReporter::error(QtCriticalMsg, this,
                           tr("Error Determining Target Document"),
                           doc, __FILE__, __LINE__))
    return;
  if (doc.first())
  {  
    params.append(doc.value("source_key_field").toString(), doc.value("docass_source_id").toInt());
    ui = doc.value("source_uiform_name").toString();
  }
  else
  {
    QMessageBox::critical(this, tr("Invalid Source"), tr("Could not determine the ui form to open"));
    return;
  }

  QWidget *w = ScriptableWidget::_guiClientInterface->openWindow(ui, params, this, 
                                                                  Qt::NonModal, Qt::Window);
  QDialog* newdlg = qobject_cast<QDialog*>(w);
  if (newdlg)
    newdlg->exec();

}

void dspDocuments::sDetach()
{
  XSqlQuery detq;

  detq.prepare("DELETE FROM docass WHERE docass_id=:docass");
  detq.bindValue(":docass", list()->id());
  detq.exec();
  ErrorReporter::error(QtCriticalMsg, this, tr("Error Deleting Assignment"),
                                detq, __FILE__, __LINE__);
  sFillList();
}
