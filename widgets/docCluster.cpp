/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "doccluster.h"

#include <QLabel>

DocCluster::DocCluster(QWidget *pParent, char const *pName) :
    VirtualCluster(pParent, pName)
{
    addNumberWidget(new DocClusterLineEdit(this, pName));
    _name->show();
}

DocClusterLineEdit::DocClusterLineEdit(QWidget* pParent, const char* pName) :
    VirtualClusterLineEdit(pParent, "files", "files_id", "files_descrip", "files_title", 0, 0, pName)
{
    setTitles(tr("Document"), tr("Documents"));
    setUiName("docAttach");
    setEditPriv("ViewDocuments");
    setViewPriv("ViewDocuments");
}
