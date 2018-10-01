/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxexemptcluster.h"

#include <QJsonDocument>
#include <QMessageBox>

#include "avalaraIntegration.h"
#include "errorReporter.h"

TaxExemptCluster::TaxExemptCluster(QWidget* parent, const char* name)
  : XComboBox(parent, name)
{
  _silent = false;
  _code = "";

  _tax = new AvalaraIntegration();
  connect(_tax, SIGNAL(taxExemptCategoriesFetched(QJsonObject, QString)), this, SLOT(sPopulateTaxExempt(QJsonObject, QString)));
  _tax->getTaxExemptCategories();
}

bool TaxExemptCluster::silent()
{
  return _silent;
}

void TaxExemptCluster::setSilent(bool silent)
{
  _silent = silent;
}

void TaxExemptCluster::setCode(const QString& code)
{
  _code = code;

  XComboBox::setCode(code);
}

void TaxExemptCluster::populate(QStringList config)
{
  _tax->getTaxExemptCategories(config);
}

void TaxExemptCluster::sPopulateTaxExempt(QJsonObject result, QString error)
{
  if (error.isEmpty())
  {
    XSqlQuery qry;
    qry.prepare("SELECT row_number() OVER (), value->>'name', value->>'code' "
                "  FROM json_array_elements((:result)::JSON->'value');");
    qry.bindValue(":result", QString::fromUtf8(QJsonDocument(result).toJson()));
    XComboBox::populate(qry);
    if (_code.isEmpty())
      setCode("TAXABLE");
    else
      setCode(_code);
    ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Tax Exempt Categories"),
                         qry, __FILE__, __LINE__);
  }
  else if (!_silent)
    QMessageBox::critical(this, tr("Avalara Error"),
                          tr("Error retrieving Avalara Tax Exempt Categories\n%1").arg(error));
}
