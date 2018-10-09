/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef XCLUSTERINPUTDIALOG_H
#define XCLUSTERINPUTDIALOG_H

#include "guiclient.h"
#include "virtualCluster.h"
#include "xdialog.h"
#include <parameter.h>

#include "ui_xclusterinputdialog.h"

class XClusterInputDialog : public XDialog, public Ui::XClusterInputDialog
{
    Q_OBJECT

public:
    XClusterInputDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~XClusterInputDialog();
    Q_INVOKABLE virtual int   getId();

public slots:
    virtual enum SetResponse set(const ParameterList & pParams );

protected slots:
    virtual void languageChange();

private:
    VirtualCluster *_cluster;
};

#endif // XCLUSTERINPUTDIALOG_H
