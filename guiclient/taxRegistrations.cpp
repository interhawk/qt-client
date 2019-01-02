/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxRegistrations.h"

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QSqlError>

#include "guiclient.h"
#include "parameterwidget.h"
#include "taxRegistration.h"
#include "errorReporter.h"
#include "xtreewidget.h"

taxRegistrations::taxRegistrations(QWidget* parent, const char*, Qt::WindowFlags fl)
  : display(parent, "taxRegistrations", fl)
{
  setWindowTitle(tr("Tax Registrations"));
  setReportName("TaxRegistrations");
  setMetaSQLOptions("taxRegistration", "detail");
  setParameterWidgetVisible(true);
  setQueryOnStartEnabled(true);

  setNewVisible(true);
  newAction()->setEnabled(_privileges->check("MaintainTaxRegistrations"));

  parameterWidget()->append(tr("Customer"), "cust_id", ParameterWidget::Customer);
  parameterWidget()->append(tr("Vendor"), "vend_id", ParameterWidget::Vendor);

  list()->addColumn(tr("Tax Zone"),       150,  Qt::AlignCenter,   true,  "taxzone_code");
  list()->addColumn(tr("Tax Authority"),  150,  Qt::AlignCenter,   true,  "taxauth_code");
  list()->addColumn(tr("Registration #"),  -1,    Qt::AlignLeft,   true,  "taxreg_number");
  list()->addColumn(tr("Start Date"),      -1,    Qt::AlignLeft,   true,  "taxreg_effective");
  list()->addColumn(tr("End Date"),        -1,    Qt::AlignLeft,   true,  "taxreg_expires");

  _custid = -1;
  _vendid = -1;

}

enum SetResponse taxRegistrations::set(const ParameterList &pParams)
{
  XWidget::set(pParams);
  QVariant param;
  bool     valid;

  parameterWidget()->setSavedFilters();

  param = pParams.value("cust_id", &valid);
  if (valid)
    parameterWidget()->setDefault(tr("Customer"), param);

  param = pParams.value("vend_id", &valid);
  if (valid)
    parameterWidget()->setDefault(tr("Vendor"), param);

  param = pParams.value("run", &valid);
  if (valid)
    sFillList();

  return NoError;
}

void taxRegistrations::sPopulateMenu(QMenu *pMenu, QTreeWidgetItem *pSelected, int)
{
  Q_UNUSED(pSelected);
  QAction *menuItem;

  menuItem = pMenu->addAction(tr("New"), this, SLOT(sNew()));
  menuItem->setEnabled(_privileges->check("MaintainTaxRegistrations"));

  menuItem = pMenu->addAction(tr("Edit"), this, SLOT(sEdit()));
  menuItem->setEnabled(_privileges->check("MaintainTaxRegistrations"));

  menuItem = pMenu->addAction(tr("View"), this, SLOT(sView()));
  menuItem->setEnabled(_privileges->check("ViewTaxRegistrations"));

  menuItem = pMenu->addAction(tr("Delete"), this, SLOT(sDelete()));
  menuItem->setEnabled(_privileges->check("MaintainTaxRegistrations"));
}

void taxRegistrations::setCustid(int custId)
{
  _custid = custId;
  if (_custid == -1)
    parameterWidget()->setDefault(tr("Customer"), QVariant(), true);
  else
    parameterWidget()->setDefault(tr("Customer"), _custid, true);
}

void taxRegistrations::setVendid(int vendId)
{
  _vendid = vendId;
  if (_vendid == -1)
    parameterWidget()->setDefault(tr("Vendor"), QVariant(), true);
  else
    parameterWidget()->setDefault(tr("Vendor"), _vendid, true);
}

void taxRegistrations::sNew()
{
  ParameterList params;
  params.append("mode", "new");

  if (_custid > 0)
  {
    params.append("taxreg_rel_id", _custid);
    params.append("taxreg_rel_type", "C");
  }
  else if (_vendid > 0)
  {
    params.append("taxreg_rel_id", _vendid);
    params.append("taxreg_rel_type", "V");
  }

  taxRegistration newdlg(this, "", true);
  newdlg.set(params);
  if(newdlg.exec() != XDialog::Rejected)
    sFillList();
}

void taxRegistrations::sEdit()
{
  ParameterList params;
  params.append("taxreg_id", list()->id());
  params.append("mode", "edit");

  taxRegistration newdlg(this, "", true);
  newdlg.set(params);
  if(newdlg.exec() != XDialog::Rejected)
    sFillList();
}

void taxRegistrations::sView()
{
  ParameterList params;
  params.append("taxreg_id", list()->id());
  params.append("mode", "view");

  taxRegistration newdlg(this, "", true);
  newdlg.set(params);
  newdlg.exec();
}

void taxRegistrations::sDelete()
{
  XSqlQuery taxDelete;
  taxDelete.prepare("DELETE FROM taxreg"
            " WHERE (taxreg_id=:taxreg_id);");
  taxDelete.bindValue(":taxreg_id", list()->id());
  taxDelete.exec();
  if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Deleting Tax Registration Information"),
                                taxDelete, __FILE__, __LINE__))
  {
    return;
  }
  sFillList();
}

