/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "avalaraIntegration.h"

#include <QApplication>
#include <QCursor>
#include <QMessageBox>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "errorReporter.h"
#include "guiclient.h"

AvalaraIntegration::AvalaraIntegration() : TaxIntegration()
{
  restclient = new QNetworkAccessManager;
  connect(restclient, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleResponse(QNetworkReply*)));
}

void AvalaraIntegration::sendRequest(QString type, QString orderType, int orderId, QString payload, QStringList config)
{
  XSqlQuery build;
  build.prepare("SELECT buildAvalaraUrl(:type, :orderType, :orderId, :url) AS url, "
                "       buildAvalaraHeaders(:localhost, :account, :key) AS headers;");
  build.bindValue(":type", type);
  build.bindValue(":orderType", orderType);
  build.bindValue(":orderId", orderId);
  build.bindValue(":localhost", QHostInfo::localHostName());
  if (config.size() >= 3)
  {
    build.bindValue(":account", config[0]);
    build.bindValue(":key", config[1]);
    build.bindValue(":url", config[2]);
  }
  build.exec();

  if (build.first())
  {
    QNetworkRequest netrequest;
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());

    netrequest.setUrl(build.value("url").toString());

    foreach(QString header, build.value("headers").toString().split(","))
    {
      netrequest.setRawHeader(header.split(": ")[0].toUtf8(), header.split(": ")[1].toUtf8());
    }

    foreach (QNetworkReply* other, replies)
    {
      if ((other->property("type") == "test" &&
           other->property("config") == config) ||
          (other->property("type") == "taxcodes") ||
          ((other->property("type") == "createtransaction" ||
            other->property("type") == "committransaction") &&
           other->property("orderType") == orderType &&
           other->property("orderId") == orderId))
      {
        other->abort();
        replies.removeOne(other);
      }
    }

    QNetworkReply* reply;
    QDateTime time;
    if (type == "test" || type == "taxcodes")
    {
      timer.start();
      time = QDateTime::currentDateTime();
      reply = restclient->get(netrequest);
    }
    else
    {
      timer.start();
      time = QDateTime::currentDateTime();
      reply = restclient->post(netrequest, doc.toJson(QJsonDocument::Compact));
    }
    reply->setProperty("type", type);
    reply->setProperty("orderType", orderType);
    reply->setProperty("orderId", orderId);
    reply->setProperty("config", config);
    reply->setProperty("request", QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    reply->setProperty("time", time);
    replies.append(reply);
  }

  ErrorReporter::error(QtCriticalMsg, 0, tr("Error building request"),
                       build, __FILE__, __LINE__);
}

void AvalaraIntegration::handleResponse(QNetworkReply* reply)
{
  int elapsed = timer.nsecsElapsed();
  QString orderType = reply->property("orderType").toString();
  int orderId = reply->property("orderId").toInt();
  QString type = reply->property("type").toString();
  QByteArray response = reply->readAll();

  if (_metrics->boolean("LogTaxService"))
  {
    XSqlQuery log;
    log.prepare("INSERT INTO taxlog "
                "(taxlog_service, taxlog_order_type, taxlog_order_id, taxlog_type, "
                " taxlog_request, taxlog_response, taxlog_start, taxlog_time) "
                " VALUES ('A', :orderType, :orderId, :type, "
                "         :request, :response, :start, :time);");
    log.bindValue(":orderType", orderType);
    log.bindValue(":orderId", orderId);
    log.bindValue(":request", reply->property("request"));
    log.bindValue(":response", QString::fromUtf8(response));
    log.bindValue(":start", reply->property("time"));
    log.bindValue(":time", elapsed);
    if (type == "test")
      log.bindValue(":type", "Ping");
    else if (type == "taxcodes")
      log.bindValue(":type", "ListTaxCodes");
    else if (type == "createtransaction")
      log.bindValue(":type", "CreateOrAdjustTransaction");
    else if (type == "committransaction")
      log.bindValue(":type", "CommitTransaction");
    else
      log.bindValue(":type", "Error");
    log.exec();
    ErrorReporter::error(QtCriticalMsg, 0, "Error logging Avalara call",
                         log, __FILE__, __LINE__);
  }

  if (reply->error() != QNetworkReply::OperationCanceledError)
  {
    QJsonObject responseJson = QJsonDocument::fromJson(response).object();
    replies.removeOne(reply);
    TaxIntegration::handleResponse(type, orderType, orderId, QString::fromUtf8(response), error(type, reply, responseJson));
  }

  delete reply;
}

QString AvalaraIntegration::error(QString type, QNetworkReply* reply, QJsonObject response)
{
  if (type == "test")
  {
    if (reply->error() != QNetworkReply::NoError)
      return reply->errorString();
    else if (!response["authenticated"].toBool())
      return tr("Invalid authentication details.");
  }
  else if (type == "taxcodes")
  {
    if (reply->error() != QNetworkReply::NoError)
      return reply->errorString();
  }
  else if (type == "createtransaction")
  {
    if (reply->error() != QNetworkReply::NoError)
    {
      QJsonObject err = response["error"].toObject();
      QString errcode = err["code"].toString();
      QString errmsg  = err["message"].toString();
      QJsonArray details = err["details"].toArray();
      QJsonObject ary = details.at(0).toObject();
      QString errhelp = ary["description"].toString();
      return tr("%1: %2<br><br>Caused by:<br>%3.").arg(errcode).arg(errmsg).arg(errhelp);
    }
  }
  else if (type == "committransaction")
  {
    if (reply->error() != QNetworkReply::NoError)
      return reply->errorString();
  }

  return "";
}

void AvalaraIntegration::wait()
{
  if (!eventLoop.isRunning() && replies.size())
  {
    qApp->setOverrideCursor(Qt::WaitCursor);
    eventLoop.exec();
  }
}

void AvalaraIntegration::done()
{
  if (replies.isEmpty())
  {
    eventLoop.quit();
    qApp->restoreOverrideCursor();
  }
}
