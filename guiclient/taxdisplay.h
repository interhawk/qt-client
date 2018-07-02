/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef TAXDISPLAY_H
#define TAXDISPLAY_H

#include "currcluster.h"
#include "taxIntegration.h"

class XTUPLEWIDGETS_EXPORT TaxDisplay : public CurrDisplay
{
  Q_OBJECT

  Q_PROPERTY(QString type READ type WRITE setType)
  Q_PROPERTY(int id READ id WRITE setId)

  public:
    TaxDisplay(QWidget* = 0, const char* = 0);

    Q_INVOKABLE QString type();
    Q_INVOKABLE int     id();

  public slots:
    void setType(QString);
    void setId(int);
    void sRecalculate();
    void sOpen();
    void sUpdate(double, QString);
    void sRefresh();

  protected:
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void resizeEvent(QResizeEvent *);
    void positionMenuLabel();

  private:
    QString         _type;
    int             _id;
    QLabel*         _menuLabel;
    QMenu*          _menu;
    QAction*        _recalculateAct;
    QAction*        _openAct;
    TaxIntegration* _tax;
};

#endif
