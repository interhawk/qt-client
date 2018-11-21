/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef CONFIGURETAX_H
#define CONFIGURETAX_H

#include "avalaraIntegration.h"
#include "xabstractconfigure.h"

#include "ui_configureTax.h"

class configureTax : public XAbstractConfigure, public Ui::configureTax
{
    Q_OBJECT

public:
    configureTax(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~configureTax();

public slots:
    virtual bool sSave();
    virtual bool sCheck();
    virtual void sTest();
    virtual void sTest(QString);
    virtual void sOpenUrl(QString);

protected slots:
    virtual void languageChange();

signals:
    void saving();

private:
  AvalaraIntegration* _tax;

};

#endif // CONFIGURETAX_H
