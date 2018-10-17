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
#include "avalaraIntegration.h"
#include "noIntegration.h"

TaxIntegration* TaxIntegration::getTaxIntegration()
{
  // Can't access _metrics from here, but only queries once at startup and in setup
  XSqlQuery service("SELECT fetchMetricText('TaxService') AS TaxService;");

  if (service.first() && service.value("TaxService") == "A")
    return new AvalaraIntegration();
  else
    return new NoIntegration();
}

TaxIntegration::TaxIntegration()
{
  if (!QSqlDatabase::database().driver()->subscribedToNotifications().contains("calculateTax"))
    QSqlDatabase::database().driver()->subscribeToNotification("calculateTax");
  if (!QSqlDatabase::database().driver()->subscribedToNotifications().contains("commit"))
    QSqlDatabase::database().driver()->subscribeToNotification("commit");
  if (!QSqlDatabase::database().driver()->subscribedToNotifications().contains("cancel"))
    QSqlDatabase::database().driver()->subscribeToNotification("cancel");

  connect(QSqlDatabase::database().driver(),
          SIGNAL(notification(const QString&, QSqlDriver::NotificationSource, const QVariant&)),
          this,
          SLOT(sNotified(const QString&, QSqlDriver::NotificationSource, const QVariant&))
         );
}

void TaxIntegration::getTaxCodes()
{
  sendRequest("taxcodes");
}

void TaxIntegration::getTaxExemptCategories(QStringList config)
{
  sendRequest("taxexempt", QString(), 0, QString(), config);
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
    if (qry.value("request").isNull())
      emit taxCalculated(0.0, "");
    else
      sendRequest("createtransaction", orderType, orderId, qry.value("request").toString());
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error calculating tax"),
                         qry, __FILE__, __LINE__);
}

void TaxIntegration::commit(QString orderType, int orderId)
{
  calculateTax(orderType, orderId, true);
  wait();

  XSqlQuery qry;
  qry.prepare("SELECT postTax(:orderType, :orderId) AS request;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.exec();
  if (qry.first() && !qry.value("request").isNull())
    sendRequest("committransaction", orderType, orderId, qry.value("request").toString());
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error posting tax transaction"),
                         qry, __FILE__, __LINE__);
}

void TaxIntegration::cancel(QString orderType, int orderId, QString orderNumber)
{
  XSqlQuery qry;
  qry.prepare("SELECT voidTax(:orderType, :orderId) AS request;");
  qry.bindValue(":orderType", orderType);
  qry.bindValue(":orderId", orderId);
  qry.exec();
  if (qry.first() && !qry.value("request").isNull())
    sendRequest("voidtransaction", orderType, orderId, qry.value("request").toString(),
                QStringList(), orderNumber);
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error voiding tax transaction"),
                         qry, __FILE__, __LINE__);
}

void TaxIntegration::refund(int invcheadId, QDate refundDate)
{
  XSqlQuery qry;
  qry.prepare("SELECT refundTax(:invcheadId, :refundDate) AS request;");
  qry.bindValue(":invcheadId", invcheadId);
  qry.bindValue(":refundDate", refundDate);
  qry.exec();
  if (qry.first() && !qry.value("request").isNull())
    sendRequest("refundtransaction", "INV", invcheadId, qry.value("request").toString());
  else
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error refunding tax"),
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
  else if (type == "taxexempt")
  {
    done();
    emit taxExemptCategoriesFetched(QJsonDocument::fromJson(response.toUtf8()).object(), error);
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

void TaxIntegration::sNotified(const QString& name, QSqlDriver::NotificationSource source, const QVariant& payload)
{
  QStringList args = payload.toString().split(",");

  if (name == "calculateTax" && args.size() >= 2)
  {
    if (args.size() == 2)
      calculateTax(args[0], args[1].toInt());
    else
      calculateTax(args[0], args[1].toInt(), true);
  }

  if (name == "commit" && args.size() >= 2)
    commit(args[0], args[1].toInt());

  if (name == "cancel" && args.size() >= 2)
  {
    if (args.size() == 2)
      cancel(args[0], args[1].toInt());
    else
      cancel(args[0], args[1].toInt(), args[2]);
  }
}

void TaxIntegration::wait()
{
}

void TaxIntegration::done()
{
}
