/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef DSPDOCUMENTS_H
#define DSPDOCUMENTS_H

#include "guiclient.h"
#include "display.h"

class dspDocuments : public display
{
    Q_OBJECT

public:
    dspDocuments(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window);

public slots:
    virtual enum SetResponse set( const ParameterList & pParams );
    virtual void sPopulateMenu(QMenu * pMenu, QTreeWidgetItem * pSelected, int pColumn);
    void sViewDoc();
    void sEditDoc();
    void sOpenAssignment();
    void sDetach();
    virtual bool setParams(ParameterList &);

protected slots:
    void sOpenDoc(QString mode = "edit");

private:
    int  _fileid;

};

#endif // DSPDOCUMENTS_H
