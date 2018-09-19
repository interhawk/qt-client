/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "exportOptions.h"
#include "exportHelper.h"
#include <QMessageBox>

ExportOptions::ExportOptions(QWidget* parent, Qt::WindowFlags fl): QDialog(parent, fl)
{
  setupUi(this);
  
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(sSave()));
  connect(csvRB, SIGNAL(clicked()), this, SLOT(sEnableCB()));
  connect(vcfRB, SIGNAL(clicked()), this, SLOT(sEnableCB()));
  connect(txtRB, SIGNAL(clicked()), this, SLOT(sEnableCB()));
  connect(htmlRB, SIGNAL(clicked()), this, SLOT(sEnableCB()));
  connect(odtRB, SIGNAL(clicked()), this, SLOT(sEnableCB()));

  populateDelim();
  _filetype = csvRB->text();

}

ExportOptions::~ExportOptions()
{
  // no need to delete child widgets, Qt does it all for us
}

void ExportOptions::sEnableCB()
{
  if(csvRB->isChecked())
  {
    delimCB->setEnabled(true);
    _filetype = csvRB->text();
  }    
  else if(vcfRB->isChecked())
  {
    delimCB->setEnabled(false);
    _filetype = vcfRB->text();
  }    
  else if(txtRB->isChecked())
  {
    delimCB->setEnabled(false);
    _filetype = txtRB->text();
  }    
  else if(htmlRB->isChecked())
  {
    delimCB->setEnabled(false);
    _filetype = htmlRB->text();
  }    
  else if(odtRB->isChecked())
  {
    delimCB->setEnabled(false);
    _filetype = odtRB->text();
  }     
}

void ExportOptions::populateDelim()
{
  QStringList delimValues;
  if(_x_preferences)
    delimValues = ExportHelper::parseDelim(_x_preferences->value("Delimiter"));
  delimCB->insertItems(0,delimValues);
}

void ExportOptions::sSave()
{
  QString errMsg;
  int delimiter = ExportHelper::validDelim(delimCB->currentText(),errMsg);
  
  if(!errMsg.isEmpty())
  {
    QMessageBox msgBox;
    msgBox.setText(errMsg);
    msgBox.exec();
  } 
  if(delimiter == ExportHelper::tooLong)
  {
    delimCB->setCurrentText(delimCB->currentText().at(0));
    return;
  }
  else if(delimiter == ExportHelper::disallowed)
    return;

  // save to DB
  QString delim;
  for (int i=0; i<delimCB->count(); i++)
    delim += delimCB->itemText(i);
  if(delim.contains(delimCB->currentText()))
    delim.remove(delimCB->currentText());
  delim.prepend(delimCB->currentText());
  _x_preferences->set("Delimiter",delim);

  _delim = delimCB->currentText();
  accept();
}