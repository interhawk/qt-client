/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef TAXREGISTRATIONS_H
#define TAXREGISTRATIONS_H

#include "display.h"

class taxRegistrations : public display
{
    Q_OBJECT

public:
    taxRegistrations(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window);

    virtual void setCustid(int custId);
    virtual void setVendid(int vendId);

public slots:
    virtual SetResponse set( const ParameterList & pParams );
    virtual void sDelete();
    virtual void sNew();
    virtual void sEdit();
    virtual void sView();
    virtual void sPopulateMenu(QMenu * pMenu, QTreeWidgetItem * pSelected, int);

private:
    int _custid;
    int _vendid;
};

#endif // TAXREGISTRATIONS_H
