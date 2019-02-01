/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "configauthorizedotnetprocessor.h"

#include <QMessageBox>
#include <QSqlError>

/** \ingroup creditcards
    \class ConfigAuthorizeDotNetProcessor
    \brief Configuration UI for the Authorize.Net credit card processor
 */

ConfigAuthorizeDotNetProcessor::ConfigAuthorizeDotNetProcessor(QWidget* parent, Qt::WindowFlags fl)
    : ConfigCreditCardProcessor(parent, fl)
{
  setupUi(this);

  connect(_anDuplicateWindow, SIGNAL(valueChanged(int)), this, SLOT(sDuplicateWindow(int)));

  if (_metrics->value("CCANVer").isEmpty())
    _anVersion->setItemText(0, "3.1");
  else
    _anVersion->setItemText(0, _metrics->value("CCANVer"));
  _anDelim->setText(_metrics->value("CCANDelim"));
  _anEncap->setText(_metrics->value("CCANEncap"));
  _anDuplicateWindow->setValue(_metrics->value("CCANDuplicateWindow").toInt());

  _anCheckSigKey->setChecked(_metrics->boolean("CCANCheckSigKey"));
  _anSigKeyWarn->setChecked(_metrics->value("CCANSigKeyAction") == "W");
  _anSigKeyFail->setChecked(_metrics->value("CCANSigKeyAction") == "F");

  if (! _metrics->value("CCANCurrency").isEmpty())
    _anCurrFixedValue->setId(_metrics->value("CCANCurrency").toInt());

  _anUsingWellsFargoSecureSource->setChecked(_metrics->boolean("CCANWellsFargoSecureSource"));
  _anIgnoreSSLErrors->setChecked(_metrics->boolean("CCANIgnoreSSLErrors"));

  if (0 != _metricsenc)
    _anSigKey->setText(_metricsenc->value("CCANSigKey"));
  else
    _anSigKey->setEnabled(false);

  sDuplicateWindow(_anDuplicateWindow->value());
}

ConfigAuthorizeDotNetProcessor::~ConfigAuthorizeDotNetProcessor()
{
  // no need to delete child widgets, Qt does it all for us
}

void ConfigAuthorizeDotNetProcessor::languageChange()
{
  retranslateUi(this);
}

bool ConfigAuthorizeDotNetProcessor::sSave()
{
  _metrics->set("CCANVer",               _anVersion->currentText());
  _metrics->set("CCANDelim",             _anDelim->text());
  _metrics->set("CCANEncap",             _anEncap->text());
  _metrics->set("CCANDuplicateWindow",   _anDuplicateWindow->cleanText());
  _metrics->set("CCANCheckSigKey",       _anCheckSigKey->isChecked());
  if (_anSigKeyWarn->isChecked())
    _metrics->set("CCANSigKeyAction", QString("W"));
  else if (_anSigKeyFail->isChecked())
    _metrics->set("CCANSigKeyAction", QString("F"));

  _metrics->set("CCANCurrency", _anCurrFixedValue->id());
  _metrics->set("CCANWellsFargoSecureSource", _anUsingWellsFargoSecureSource->isChecked());
  _metrics->set("CCANIgnoreSSLErrors",     _anIgnoreSSLErrors->isChecked());

  _metrics->load();

  if (0 != _metricsenc)
  {
    _metricsenc->set("CCANSigKey",     _anSigKey->text());
    _metricsenc->load();
  }

  return true;
}

void ConfigAuthorizeDotNetProcessor::sDuplicateWindow(int p)
{
  QTime time;
  (void)time.addSecs(p);
  _anDuplicateWindowAsHMS->setText(time.toString("HH:mm:ss"));
}
