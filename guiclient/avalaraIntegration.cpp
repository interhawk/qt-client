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

  urlmap.insert("test",              "utilities/ping");
  urlmap.insert("taxcodes",          "definitions/taxcodes");
  urlmap.insert("createtransaction", "transactions/create");
  urlmap.insert("committransaction", "companies/{companyCode}/transactions/{transactionCode}/commit");
}

QUrl AvalaraIntegration::buildUrl(QString api, QString transcode = "")
{
  QString url = _metrics->value("AvalaraUrl") + "api/v2/" + urlmap.value(api);

  url.replace("{companyCode}", _metrics->value("AvalaraCompany"), Qt::CaseSensitive);
  url.replace("{transactionCode}", transcode, Qt::CaseSensitive);
  
  return QUrl(url); 
}

void AvalaraIntegration::buildHeaders(QNetworkRequest &netrequest)
{
  QByteArray authkey;
  QByteArray client;
  QString localhost = QHostInfo::localHostName();

  authkey.append(buildAuthKey());
  client.append(QString("xTuple; %1; REST; V2; %2").arg(_metrics->value("ServerVersion"))
                                                  .arg(localhost));

  netrequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
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

bool AvalaraIntegration::testService()
{
  QEventLoop eventLoop;
  QNetworkRequest netrequest;

  netrequest.setUrl(buildUrl("test"));
  buildHeaders(netrequest);

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
     {
       QMessageBox::information(0, tr("Avalara Integration Test"),
                    tr("<p>Avalara Integration Test Failed<br>"
                       "Invalid authentication details."));

        delete reply;
        return false;
     } 
  }
  else
  {
    QMessageBox::information(0, tr("Avalara Integration Test"),
                  tr("<p>Avalara Integration Test Failed<br>"
                     "%1").arg(reply->errorString()));
     delete reply;
     return false;
  }

  delete reply;
  return true;
}

QJsonObject AvalaraIntegration::sendRequest(QJsonObject payload)
{
  QEventLoop eventLoop;
  QNetworkRequest netrequest;
  QJsonObject response;
  QJsonDocument doc(payload);
  
  netrequest.setUrl(buildUrl("createtransaction"));
  buildHeaders(netrequest);

  connect(restclient, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

  QNetworkReply *reply = restclient->post(netrequest, doc.toJson(QJsonDocument::Compact));
  eventLoop.exec();

  if (reply->error() == QNetworkReply::NoError)
  {
     QString strReply = (QString)reply->readAll();
     QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
     response = jsonResponse.object();
  }
  else
  {
    QMessageBox::information(0, tr("Avalara Create Transaction"),
                  tr("<p>Avalara Create Transaction Failed<br>"
                     "%1").arg(reply->errorString()));
  }

  delete reply;
  return response;
}

QJsonObject AvalaraIntegration::getTaxCodeList()
{
  QEventLoop eventLoop;
  QNetworkRequest netrequest;
  QJsonObject response;
  QJsonObject jsonObject;

  netrequest.setUrl(buildUrl("taxcodes"));
  buildHeaders(netrequest);

  connect(restclient, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

  QNetworkReply *reply = restclient->get(netrequest);
  eventLoop.exec();

  //Convert Avalara reply to xTuple requirements
  if (reply->error() == QNetworkReply::NoError)
  {
     QString strReply = (QString)reply->readAll();
     QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
     jsonObject = jsonResponse.object();
  }
  else
  {
    QMessageBox::information(0, tr("Avalara Tax Codes"),
                  tr("<p>Error retrieving Avalara Tax Codes<br>"
                     "%1").arg(reply->errorString()));
  }

  delete reply;
  return jsonObject;
}

void AvalaraIntegration::commitTransaction(QString transcode)
{
  QEventLoop eventLoop;
  QNetworkRequest netrequest;

  netrequest.setUrl(buildUrl("committransaction", transcode));
  buildHeaders(netrequest);

  QVariantMap commit;
  commit.insert("commit", true);
  QByteArray payload = QJsonDocument::fromVariant(commit).toJson();

  connect(restclient, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

  QNetworkReply *reply = restclient->post(netrequest, payload);
  eventLoop.exec();

  if (reply->error() != QNetworkReply::NoError)
    QMessageBox::information(0, tr("Avalara Tax Service"),
                tr("<p>Error Committing Avalara Transaction<br>"
                   "%1").arg(reply->errorString()));

  delete reply;
  return;
}
