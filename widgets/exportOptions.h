/********************************************************************************
** Form generated from reading UI file 'exportOptions.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef EXPORTOPTIONS_H
#define EXPORTOPTIONS_H


#include <QDialog>

#include "ui_exportOptions.h"
#include "xtreewidget.h"

QT_BEGIN_NAMESPACE

class ExportOptions: public QDialog , public Ui::ExportOptions
{
  Q_OBJECT
  public:
    ExportOptions(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~ExportOptions();
   
    void populateDelim();
    QString getFiletype(){return _filetype;}
    QString getDelim(){return _delim;} 

  public slots:
    void sEnableCB();  
    void sSave();

  private:
    QString _filetype;
    QString _delim;

};

QT_END_NAMESPACE

#endif 
