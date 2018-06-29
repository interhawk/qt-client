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

#include "errorReporter.h"
#include "guiclient.h"
#include "avalaraIntegration.h"
#include "noIntegration.h"

TaxIntegration* TaxIntegration::getTaxIntegration()
{
  if (_metrics->value("TaxService") == "A")
    return new AvalaraIntegration();
  else
    return new NoIntegration();
}

void TaxIntegration::getTaxCodes()
{
  sendRequest("taxcodes");
}

void TaxIntegration::test(QStringList config)
{
  sendRequest("test", QString(), 0, QJsonObject(), config);
}

void TaxIntegration::calculateTax(QString orderType, int orderId)
{
  XSqlQuery qry;
  qry.prepare("SELECT calculateOrderTax(:orderType, :orderId) AS request;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.exec();
  if (qry.first())
  {
    QJsonObject request = QJsonDocument::fromJson(qry.value("request").toString().toUtf8()).object();
    sendRequest("createtransaction", orderType, orderId, request);
  }
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error calculating tax"),
                         qry, __FILE__, __LINE__);
}

void TaxIntegration::handleResponse(QString type, QString orderType, int orderId, QJsonObject response, QString error)
{
  if (type == "test")
  {
    done();
    emit connectionTested(error);
  }
  else if (type=="taxcodes")
  {
    done();
    emit taxCodesFetched(response, error);
  }
  else if (type=="createtransaction")
  {
    if (error.isEmpty())
    {
      XSqlQuery qry;
      qry.prepare("SELECT saveTax(:orderType, :orderId, :response) AS tax;");
      qry.bindValue(":orderType", orderType);
      qry.bindValue(":orderId", orderId);
      qry.bindValue(":response", QString::fromUtf8(QJsonDocument(response).toJson()));
      qry.exec();
      if (qry.first())
      {
        done();
        emit taxCalculated(qry.value("tax").toDouble(), error);
      }
      else
        ErrorReporter::error(QtCriticalMsg, 0, tr("Error calculating tax"),
                             qry, __FILE__, __LINE__);
    }
    else
      emit (0.0, error);
  }
}

void TaxIntegration::wait()
{
}

void TaxIntegration::done()
{
}
