/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxIntegration.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "guiclient.h"
#include "errorReporter.h"
#include "avalaraIntegration.h"
#include "noIntegration.h"

TaxIntegration* TaxIntegration::getTaxIntegration()
{
  if (_metrics->value("TaxService") == "A")
    return new AvalaraIntegration();
  else
    return new NoIntegration();
}

double TaxIntegration::calculateTax(QString orderType, int orderId)
{
  QJsonObject response;

  XSqlQuery qry;
  qry.prepare("SELECT calculateOrderTax(:orderType, :orderId) AS request;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.exec();
  if (qry.first())
  {
    QJsonObject request = QJsonDocument::fromJson(qry.value("request").toString().toUtf8()).object();
    response = sendRequest(request);
  }
  else if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error calculating tax"),
                                qry, __FILE__, __LINE__))
    return 0.0;

  //validate response

  qry.prepare("SELECT saveTax(:orderType, :orderId, :response) AS tax;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.bindValue(":response", QString::fromUtf8(QJsonDocument(response).toJson()));
  qry.exec();
  if (qry.first())
    return qry.value("tax").toDouble();
  else if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error calculating tax"),
                                qry, __FILE__, __LINE__))
    return 0.0;
}

QJsonObject TaxIntegration::getTaxCodes()
{
  return getTaxCodeList();
}

