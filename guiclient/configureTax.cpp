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

#include <QMessageBox>

configureTax::configureTax(QWidget* parent, const char* name, bool /*modal*/, Qt::WindowFlags fl)
    : XAbstractConfigure(parent, fl)
{
  setupUi(this);

  _service->append(0, "None",    "N");
  _service->append(1, "Avalara", "A");

  connect(_account, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_key, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_url, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_company, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_test, SIGNAL(clicked()), this, SLOT(sTest()));

  if (_metrics->value("TaxService") != "")
    _service->setCode(_metrics->value("TaxService"));

  _account->setText(_metrics->value("AvalaraAccount"));
  _key->setText(_metrics->value("AvalaraKey"));
  _url->setText(_metrics->value("AvalaraUrl"));
  _company->setText(_metrics->value("AvalaraCompany"));
  _disableRecording->setChecked(_metrics->boolean("NoAvaTaxCommit"));

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
  }
  else
  {
    _metrics->set("AvalaraAccount", QString());
    _metrics->set("AvalaraKey", QString());
    _metrics->set("AvalaraUrl", QString());
    _metrics->set("AvalaraCompany", QString());
    _metrics->set("NoAvaTaxCommit", false);
  }

  return true;
}

bool configureTax::sCheck()
{
  bool valid = (!_account->isNull() && !_key->isNull() && !_url->isNull() && !_company->isNull());
  _test->setEnabled(valid);

  return valid;
}

bool configureTax::sTest()
{
  AvalaraIntegration ai;
  ai.test();

  return false;
}
