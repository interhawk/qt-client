/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "configureTax.h"
#include "avalaraIntegration.h"
#include "errorReporter.h"

#include <QMessageBox>

configureTax::configureTax(QWidget* parent, const char* name, bool /*modal*/, Qt::WindowFlags fl)
    : XAbstractConfigure(parent, fl)
{
  setupUi(this);

  _tax = new AvalaraIntegration();

  _service->append(0, "None",    "N");
  _service->append(1, "Avalara", "A");

  connect(_account, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_key, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_url, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_company, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_log, SIGNAL(toggled(bool)), this, SLOT(sLog(bool)));
  connect(_test, SIGNAL(clicked()), this, SLOT(sTest()));
  connect(_tax, SIGNAL(connectionTested(QString)), this, SLOT(sTest(QString)));

  if (_metrics->value("TaxService") != "")
    _service->setCode(_metrics->value("TaxService"));

  _account->setText(_metrics->value("AvalaraAccount"));
  _key->setText(_metrics->value("AvalaraKey"));
  _url->setText(_metrics->value("AvalaraUrl"));
  _company->setText(_metrics->value("AvalaraCompany"));
  _disableRecording->setChecked(_metrics->boolean("NoAvaTaxCommit"));
  _log->setChecked(_metrics->boolean("LogTaxService"));

  sCheck();
}

configureTax::~configureTax()
{
  // no need to delete child widgets, Qt does it all for us
}

void configureTax::languageChange()
{
  retranslateUi(this);
}

bool configureTax::sSave()
{
  emit saving();

  _metrics->set("TaxService", _service->code());

  if (_service->code() == "A")
  {
    _metrics->set("AvalaraAccount", _account->text());
    _metrics->set("AvalaraKey", _key->text());
    _metrics->set("AvalaraUrl", _url->text());
    _metrics->set("AvalaraCompany", _company->text());
    _metrics->set("NoAvaTaxCommit", _disableRecording->isChecked());
    _metrics->set("LogTaxService", _log->isChecked());
  }

  return true;
}

bool configureTax::sCheck()
{
  bool valid = (!_account->isNull() && !_key->isNull() && !_url->isNull() && !_company->isNull());
  _test->setEnabled(valid);

  return valid;
}

void configureTax::sLog(bool log)
{
  if (!log && QMessageBox::question(this, tr("Clear Log?"),
                                    tr("Do you wish to clear all logged data?"),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No) == QMessageBox::Yes)
  {
    XSqlQuery del;
    del.prepare("DELETE FROM taxlog "
                " WHERE taxlog_service = :service;");
    del.bindValue(":service", _service->code());
    del.exec();
    ErrorReporter::error(QtCriticalMsg, this, tr("Error clearing log"),
                         del, __FILE__, __LINE__);
  }
}

void configureTax::sTest()
{
  QStringList configuration;
  configuration << _account->text() << _key->text() << _url->text() << _company->text();

  _tax->test(configuration);
  _tax->wait();

  return;
}

void configureTax::sTest(QString error)
{
  if (error.isEmpty())
    QMessageBox::information(this, tr("Avalara Integration Test"), tr("Connection Test Succeeded"));
  else
    QMessageBox::information(this, tr("Avalara Integration Test"), tr("Connection Test Failed:\n%1").arg(error));
}
