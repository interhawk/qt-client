/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef PRINTCHECKSREVIEWEDIT_H
#define PRINTCHECKSREVIEWEDIT_H

#include "xdialog.h"
#include "ui_printChecksReviewEdit.h"

class printChecksReviewEdit : public XDialog, public Ui::printChecksReviewEdit
{
    Q_OBJECT

public:
    printChecksReviewEdit(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~printChecksReviewEdit();

public slots:
    virtual enum SetResponse set(const ParameterList & pParams );
    virtual void sSave();

protected slots:
    virtual void languageChange();

private:
    int _id;
};

#endif // PRINTCHECKSREVIEWEDIT_H
