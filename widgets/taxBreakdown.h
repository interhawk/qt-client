/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef TAXBREAKDOWN_H
#define TAXBREAKDOWN_H

#include <parameter.h>
#include "ui_taxBreakdown.h"

class XTUPLEWIDGETS_EXPORT taxBreakdown : public QDialog, public Ui::taxBreakdown
{
  Q_OBJECT

  public:
    taxBreakdown(QWidget* = 0, const char* = 0, bool = 0, Qt::WindowFlags = 0);
    ~taxBreakdown();

  public slots:
    void	     languageChange();

    virtual void        set(const ParameterList&);
    virtual void	sPopulate();
    virtual void        sHandleButtons(bool);
    virtual void        sNew();
    virtual void        sDelete();

  private:
    int         _mode;
    int		_orderid;
    int         _sense;
    QString	_ordertype;
    QDate       _date;
};
#endif // TAXBREAKDOWN_H
