/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef AVALARAINTEGRATION_H
#define AVALARAINTEGRATION_H

#include "taxIntegration.h"

#include <QNetworkAccessManager>
#include <QEventLoop>

class AvalaraIntegration : public TaxIntegration
{
  Q_OBJECT

  public:
    AvalaraIntegration();
    void wait();

  protected:
    virtual void        sendRequest(QString, QString, int, QJsonObject, QStringList);
    virtual QString     error(QString, QNetworkReply*, QJsonObject);
    virtual void        done();

  protected slots:
    virtual void        handleResponse(QNetworkReply*);

  private:
    QNetworkAccessManager * restclient;
    QList<QNetworkReply*> replies;
    QEventLoop eventLoop;
};

#endif
