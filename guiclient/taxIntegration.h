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

#include <QObject>

class TaxIntegration : public QObject
{
  Q_OBJECT

  public:
    Q_INVOKABLE static TaxIntegration* getTaxIntegration();
    Q_INVOKABLE virtual double calculateTax(QString, int);

  protected:
    virtual QJsonObject sendRequest(QJsonObject) = 0;
};

#endif
