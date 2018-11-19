/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "configureEncryption.h"
#include "guiErrorCheck.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QInputDialog>

extern bool shownEncryptedMsg;

configureEncryption::configureEncryption(QWidget* parent, const char* name, bool /*modal*/, Qt::WindowFlags fl)
    : XAbstractConfigure(parent, fl)
{
  setupUi(this);

  if (name)
    setObjectName(name);

  if (_metricsenc == 0 && !shownEncryptedMsg)
  {
    QMessageBox::critical( this, tr("Cannot Read Configuration"),
		    tr("<p>Cannot read encrypted information from database."));
    shownEncryptedMsg = true;
  }

  _ccEncKeyName->setText(_metrics->value("CCEncKeyName"));
  _ccWinEncKey->setText(_metrics->value("CCWinEncKey"));
  _ccLinEncKey->setText(_metrics->value("CCLinEncKey"));
  _ccMacEncKey->setText(_metrics->value("CCMacEncKey"));

  if (_ccEncKeyName->text().trimmed().isEmpty())
    connect(_createFile, SIGNAL(clicked()), this, SLOT(sCreate()));
  else
  {
    _originalKey = _ccEncKeyName->text();
    _createFile->setEnabled(false);
  }

  connect(_ccEncKeyName, SIGNAL(editingFinished()), this, SLOT(sKeyChange()));
}

configureEncryption::~configureEncryption()
{
  // no need to delete child widgets, Qt does it all for us
}

void configureEncryption::languageChange()
{
  retranslateUi(this);
}

bool configureEncryption::sSave()
{
  emit saving();

  QList<GuiErrorCheck> errors;

  errors << GuiErrorCheck(_ccEncKeyName->text().trimmed().isEmpty(), _ccEncKeyName, tr("Enter a Key File Name"))
         << GuiErrorCheck((_ccWinEncKey->text().trimmed().isEmpty()
                           && _ccLinEncKey->text().trimmed().isEmpty()
                           && _ccMacEncKey->text().trimmed().isEmpty()), _ccWinEncKey, tr("Enter a File Location"));

  if (GuiErrorCheck::reportErrors(this, tr("Encryption error"), errors))
      return false;

  _metrics->set("CCEncKeyName",      _ccEncKeyName->text());
  _metrics->set("CCWinEncKey",       _ccWinEncKey->text());
  _metrics->set("CCLinEncKey",       _ccLinEncKey->text());
  _metrics->set("CCMacEncKey",       _ccMacEncKey->text());

  _metrics->load();

  if (0 != _metricsenc)
    _metricsenc->load();

  return true;
}

void configureEncryption::sCreate()
{
  QString filename;
  QString dir;
  QString defdir;
  QString tempkey;
  QString key;

  if (_ccEncKeyName->text().trimmed().isEmpty())
  {
    _originalKey = "xTuple.key";
    _ccEncKeyName->setText(_originalKey);
  }

#ifdef Q_OS_WIN
  defdir = _metrics->value("CCWinEncKey");
#elif defined Q_OS_MAC
  defdir = _metrics->value("CCMacEncKey");
#else
  defdir = _metrics->value("CCLinEncKey");
#endif

  dir = QFileDialog::getExistingDirectory(this, tr("Encryption Key Directory"),
                                          defdir,
                                          QFileDialog::ShowDirsOnly
                                          | QFileDialog::DontResolveSymlinks);
  filename = dir + QDir::separator() + _ccEncKeyName->text().trimmed();
  QFileInfo fi(filename);
  if(fi.suffix().isEmpty())
    filename += ".key";

  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly))
  {
    QMessageBox::critical(0, tr("Could not create key file"), file.errorString());
    return;
  }

  // User enters a key for the file or we generate a hash for them
  bool ok;
  QDateTime d =  QDateTime::currentDateTime();
  tempkey = d.toString(QString("yyyyMMMddhhmmss"));
  if (_metrics->value("DatabaseName").size() > 5)
    tempkey = tempkey + _metrics->value("DatabaseName");
  tempkey = QString(QCryptographicHash::hash(tempkey.toLocal8Bit(),QCryptographicHash::Md5).toHex());
  tempkey = QInputDialog::getText(this, tr("Encryption Key"),
                              tr("A key has been generated for your file. Please select this key or enter one of your own."), QLineEdit::Normal,
                              tempkey, &ok);
  if (ok && !tempkey.isEmpty())
    key = tempkey;
  else
  {
    QMessageBox::information( this, tr("Encryption Key Required"),
                              tr("The file must have valid Encryption Key text entered."));
    _ccEncKeyName->setText("");
    return;
  }

  // Write key to open file
  QTextStream ts(&file);
  ts.setCodec("UTF-8");
  ts << key;
  file.close();

  QMessageBox::information( this, tr("xTuple Key File"),
                         tr("<p>The xTuple key file was generated. You will need to distribute the "
                            "file to all users who need access to encrypted information (such as credit card or EFT information). "
                            "The file must be located on each desktop in the directory specified on this screen. "
                            "Once the key file has been used you cannot change the file without risking loss of data."));
  return;
}

void configureEncryption::sKeyChange()
{
  if (_originalKey == _ccEncKeyName->text() || _originalKey.isEmpty())
    return;

  if (QMessageBox::question( this, tr("xTuple Key File"),
                         tr("<p>The xTuple key file name has changed. "
                            "Once the key file has been used you cannot change the file without risking loss of data. "
                            "Do you wish to proceed?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
  {
    _ccEncKeyName->setText(_originalKey);
  }
}

