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

AvalaraIntegration::AvalaraIntegration() : TaxIntegration()
{
  restclient = new QNetworkAccessManager;
  connect(restclient, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleResponse(QNetworkReply*)));
}

void AvalaraIntegration::sendRequest(QString type, QString orderType, int orderId, QJsonObject payload, QStringList config)
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
    QJsonDocument doc(payload);

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
          (other->property("type") == "createtransaction" &&
           other->property("orderType") == orderType &&
           other->property("orderId") == orderId))
      {
        other->abort();
        replies.removeOne(other);
      }
    }

    QNetworkReply* reply;
    if (type == "test" || type == "taxcodes")
      reply = restclient->get(netrequest);
    else
      reply = restclient->post(netrequest, doc.toJson(QJsonDocument::Compact));
    reply->setProperty("type", type);
    reply->setProperty("orderType", orderType);
    reply->setProperty("orderId", orderId);
    reply->setProperty("config", config);
    replies.append(reply);
  }

  ErrorReporter::error(QtCriticalMsg, 0, tr("Error building request"),
                       build, __FILE__, __LINE__);
}

void AvalaraIntegration::handleResponse(QNetworkReply* reply)
{
  if (reply->error() != QNetworkReply::OperationCanceledError)
  {
    QJsonObject response = QJsonDocument::fromJson(reply->readAll()).object();
    replies.removeOne(reply);
    TaxIntegration::handleResponse(reply->property("type").toString(), reply->property("orderType").toString(), reply->property("orderId").toInt(), response, error(reply->property("type").toString(), reply, response));
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
