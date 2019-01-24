/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef EXPORTOPTIONS_H
#define EXPORTOPTIONS_H


#include <QDialog>

#include "ui_exportOptions.h"
#include "xtreewidget.h"
#include "exporthelper.h"

QT_BEGIN_NAMESPACE

class ExportOptions: public QDialog , public Ui::ExportOptions
{
  Q_OBJECT
  public:
    ExportOptions(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~ExportOptions();
   
    void populateDelim();
    QString getFiletype();
    QString getDelim();
    bool getOpen();

  public slots:
    void sEnableCB();  
    void sSave();

  private:
    QString _filetype;
    QString _delim;
    bool _open;
};

QT_END_NAMESPACE

#endif 
