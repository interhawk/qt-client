/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef DSPPODELIVERYDATEVARIANCESBYVENDOR_H
#define DSPPODELIVERYDATEVARIANCESBYVENDOR_H

#include "display.h"

#include "ui_dspPoDeliveryDateVariancesByVendor.h"

class dspPoDeliveryDateVariancesByVendor : public display, public Ui::dspPoDeliveryDateVariancesByVendor
{
    Q_OBJECT

public:
    dspPoDeliveryDateVariancesByVendor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window);

    virtual bool setParams(ParameterList &);

protected slots:
    virtual void languageChange();

};

#endif // DSPPODELIVERYDATEVARIANCESBYVENDOR_H
