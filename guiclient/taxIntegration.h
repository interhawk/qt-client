/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef TAXINTEGRATION_H
#define TAXINTEGRATION_H

#include <QElapsedTimer>
#include <QObject>
#include <QJsonObject>

#include "xsqlquery.h"

class TaxIntegration : public QObject
{
  Q_OBJECT

  public:
    Q_INVOKABLE static TaxIntegration* getTaxIntegration();
    Q_INVOKABLE virtual void test(QStringList);
    Q_INVOKABLE virtual void getTaxCodes();
    Q_INVOKABLE virtual void calculateTax(QString, int, bool = false);
    Q_INVOKABLE virtual void commit(QString, int);
    Q_INVOKABLE virtual void cancel(QString, int);
    Q_INVOKABLE virtual void wait();

  signals:
    void connectionTested(QString);
    void taxCodesFetched(QJsonObject, QString);
    void taxCalculated(double, QString);

  protected:
    virtual void sendRequest(QString, QString = QString(), int = 0, QString = QString(), QStringList = QStringList()) = 0;
    virtual void done();

    QElapsedTimer timer;

  protected slots:
    virtual void handleResponse(QString, QString, int, QString, QString);
};

#endif
