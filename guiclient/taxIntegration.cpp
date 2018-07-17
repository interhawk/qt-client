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
  sendRequest("test", QString(), 0, QString(), config);
}

void TaxIntegration::calculateTax(QString orderType, int orderId, bool record)
{
  XSqlQuery qry;
  qry.prepare("SELECT calculateOrderTax(:orderType, :orderId, :record) AS request;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.bindValue(":record", record);
  qry.exec();
  if (qry.first())
    sendRequest("createtransaction", orderType, orderId, qry.value("request").toString());
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error calculating tax"),
                         qry, __FILE__, __LINE__);
}

void TaxIntegration::commit(QString orderType, int orderId)
{
  XSqlQuery qry;
  qry.prepare("SELECT postTax(:orderType, :orderId) AS request;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.exec();
  if (qry.first())
    sendRequest("committransaction", orderType, orderId, qry.value("request").toString());
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error posting tax transaction"),
                         qry, __FILE__, __LINE__);
}

void TaxIntegration::handleResponse(QString type, QString orderType, int orderId, QString response, QString error)
{
  if (type == "test")
  {
    done();
    emit connectionTested(error);
  }
  else if (type=="taxcodes")
  {
    done();
    emit taxCodesFetched(QJsonDocument::fromJson(response.toUtf8()).object(), error);
  }
  else if (type=="createtransaction")
  {
    if (error.isEmpty())
    {
      XSqlQuery qry;
      qry.prepare("SELECT saveTax(:orderType, :orderId, :response) AS tax;");
      qry.bindValue(":orderType", orderType);
      qry.bindValue(":orderId", orderId);
      qry.bindValue(":response", response);
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
      emit taxCalculated(0.0, error);
  }
}

void TaxIntegration::wait()
{
}

void TaxIntegration::done()
{
}
