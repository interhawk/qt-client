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

#include <QJsonObject>
#include <QMessageBox>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>

AvalaraIntegration::AvalaraIntegration() : TaxIntegration()
{
  restclient = new QNetworkAccessManager;
}

QJsonObject AvalaraIntegration::sendRequest(QJsonObject request)
{

  QJsonObject response;

  //Send request, return response

  return response;
}

QUrl AvalaraIntegration::buildUrl(QString api)
{
  QString base = _metrics->value("AvalaraUrl") + "api/v2/";

  if (api == "test")
    return QUrl( base + "utilities/ping");
}

void AvalaraIntegration::buildHeaders(QNetworkRequest &netrequest)
{
  QByteArray authkey;
  QByteArray client;
  QString localhost = QHostInfo::localHostName();

  authkey.append(buildAuthKey());
  client.append(QString("xTuple; %1; REST; V2; %2").arg(_metrics->value("ServerVersion"))
                                                  .arg(localhost));

  netrequest.setRawHeader("Authorization", authkey);
  netrequest.setRawHeader("X-Avalara-UID", "a0o0b000003PfVt");
  netrequest.setRawHeader("X-Avalara-Client", client);
  return;
}

QString AvalaraIntegration::buildAuthKey()
{
  QString account = _metrics->value("AvalaraAccount");
  QString key     = _metrics->value("AvalaraKey");
  QString auth    = QString("%1:%2").arg(account).arg(key);
  QString enc     = auth.toUtf8().toBase64();

  return "Basic " + enc;
}

void AvalaraIntegration::test()
{
  QEventLoop eventLoop;
  QNetworkRequest netrequest;
  QByteArray authkey;
  QByteArray client;
  QString localhost = QHostInfo::localHostName();

  buildHeaders(netrequest);
  netrequest.setUrl(buildUrl("test"));

  connect(restclient, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

  QNetworkReply *reply = restclient->get(netrequest);
  eventLoop.exec();

  if (reply->error() == QNetworkReply::NoError)
  {
     QString strReply = (QString)reply->readAll();
     QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
     QJsonObject jsonObj = jsonResponse.object();
     QString authd = jsonObj["authenticated"].toBool() ? "true" : "false";
     QString authAccnt = QString::number(jsonObj["authenticatedAccountId"].toInt());

     if (jsonObj["authenticated"].toBool())
       QMessageBox::information(0, tr("Avalara Integration Test"),
                    tr("<p>Avalara Integration Test Successful<br>"
                       "Authenticated: %1 <br>"
                       "Authenticated Account Id: %2")
                    .arg(authd)
                    .arg(authAccnt));
     else
       QMessageBox::information(0, tr("Avalara Integration Test"),
                    tr("<p>Avalara Integration Test Failed<br>"
                       "Invalid authentication details."));

     delete reply;
  }
  else
  {
    QMessageBox::information(0, tr("Avalara Integration Test"),
                  tr("<p>Avalara Integration Test Failed<br>"
                     "%1").arg(reply->errorString()));
     delete reply;
  }
  return;
}
