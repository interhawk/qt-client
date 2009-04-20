/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxclasses.h"

#include <QVariant>
#include <QMessageBox>
//#include <QStatusBar>
#include <parameter.h>
#include <openreports.h>
#include "taxClass.h"

/*
 *  Constructs a taxClasses as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
taxClasses::taxClasses(QWidget* parent, const char* name, Qt::WFlags fl)
    : XWidget(parent, name, fl)
{
  setupUi(this);

//  (void)statusBar();

  // signals and slots connections
  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
  connect(_close, SIGNAL(clicked()), this, SLOT(close()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
  connect(_view, SIGNAL(clicked()), this, SLOT(sView()));
  connect(_taxclass, SIGNAL(valid(bool)), _view, SLOT(setEnabled(bool)));

//  statusBar()->hide();

 _taxclass->addColumn(tr("Class"),        150, Qt::AlignLeft,  true,  "taxclass_code" );
 _taxclass->addColumn(tr("Description"), 300, Qt::AlignLeft,  true,  "taxclass_descrip" );
 _taxclass->addColumn(tr("Group Sequence"), -1, Qt::AlignLeft,  true,  "taxclass_sequence" );

  if (_privileges->check("MaintainTaxClasses"))
  {
    connect(_taxclass, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
    connect(_taxclass, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    connect(_taxclass, SIGNAL(itemSelected(int)), _edit, SLOT(animateClick()));
  }
  else
  {
    connect(_taxclass, SIGNAL(itemSelected(int)), _view, SLOT(animateClick()));

    _new->setEnabled(FALSE);
  }

  sFillList(-1);
}

/*
 *  Destroys the object and frees any allocated resources
 */
taxClasses::~taxClasses()
{
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void taxClasses::languageChange()
{
  retranslateUi(this);
}

void taxClasses::sDelete()
{
  q.prepare("SELECT deletetaxclass(:tax_class_id) AS result;");
  q.bindValue(":tax_class_id", _taxclass->id());
  q.exec();
  if (q.first())
  {
    if (q.value("result").toInt() < 0)
    {
      QMessageBox::warning( this, tr("Cannot Delete Tax Class"),
                            tr( "You cannot delete the selected Tax Class because there are currently items assigned to it.\n"
                                "You must first re-assign these items before deleting the selected Tax Class." ) );
      return;
    }

    sFillList(-1);
  }
  else
    systemError(this, tr("A System Error occurred at %1::%2.")
                      .arg(__FILE__)
                      .arg(__LINE__) );
}

void taxClasses::sNew()
{
  ParameterList params;
  params.append("mode", "new");

  taxClass newdlg(this, "", TRUE);
  newdlg.set(params);
  
  int result = newdlg.exec();
  if (result != XDialog::Rejected)
    sFillList(result);
}

void taxClasses::sEdit()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("taxclass_id", _taxclass->id());

  taxClass newdlg(this, "", TRUE);
  newdlg.set(params);
  
  int result = newdlg.exec();
  if (result != XDialog::Rejected)
    sFillList(result);
}

void taxClasses::sView()
{
  ParameterList params;
  params.append("mode", "view");
  params.append("taxclass_id", _taxclass->id());

  taxClass newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

void taxClasses::sPrint()
{
  orReport report("TaxClassesMasterList");
  if (report.isValid())
    report.print();
  else
    report.reportError(this);
}

void taxClasses::sFillList(int pId)
{
	_taxclass->populate( "SELECT taxclass_id, taxclass_code, taxclass_descrip, taxclass_sequence "
                      "FROM taxclass "
                      "ORDER BY taxclass_sequence;", pId );
}
