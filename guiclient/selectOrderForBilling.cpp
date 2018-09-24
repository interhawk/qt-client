/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2014 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "selectOrderForBilling.h"

#include <QMessageBox>
#include <QSqlError>
//#include <QStatusBar>
#include <QVariant>

#include <metasql.h>

#include "guiErrorCheck.h"
#include "salesOrder.h"
#include "salesOrderList.h"
#include "selectBillingQty.h"
#include "storedProcErrorLookup.h"
#include "taxIntegration.h"
#include "errorReporter.h"

selectOrderForBilling::selectOrderForBilling(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : XWidget(parent, name, fl)
{
  setupUi(this);

//  (void)statusBar();

  _so->setAllowedTypes(OrderLineEdit::Sales);
  _so->setAllowedStatuses(OrderLineEdit::Open);

  connect(_cancel, SIGNAL(clicked()), this, SLOT(sCancelSelection()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEditOrder()));
  connect(_freightTaxtype, SIGNAL(newID(int)), this, SLOT(sMiscTaxtypeChanged()));
  connect(_freight, SIGNAL(valueChanged()), this, SLOT(sFreightChanged()));
  connect(_miscChargeTaxtype, SIGNAL(newID(int)), this, SLOT(sMiscTaxtypeChanged()));
  connect(_miscChargeDiscount, SIGNAL(toggled(bool)), this, SLOT(sMiscTaxtypeChanged()));
  connect(_miscChargeAccount, SIGNAL(valid(bool)), this, SLOT(sMiscTaxtypeChanged()));
  connect(_miscCharge, SIGNAL(valueChanged()), this, SLOT(sMiscChargeChanged()));
  connect(_save, SIGNAL(clicked()), this, SLOT(sSave()));
  connect(_select, SIGNAL(clicked()), this, SLOT(sEditSelection()));
  connect(_selectBalance, SIGNAL(clicked()), this, SLOT(sSelectBalance()));
  connect(_showClosed, SIGNAL(toggled(bool)), this, SLOT(sFillList()));
  connect(_so, SIGNAL(newId(int,QString)), this, SLOT(sPopulate(int)));
  connect(_salesTax,    SIGNAL(save(bool)),     this, SLOT(save(bool)));
  connect(_salesTax,	SIGNAL(valueChanged()),	this, SLOT(sUpdateTotal()));
  connect(_subtotal,	SIGNAL(valueChanged()),	this, SLOT(sUpdateTotal()));
  connect(_taxZone,	SIGNAL(newID(int)),	this, SLOT(sTaxZoneChanged()));
  
  _cobmiscid = -1;
  _taxzoneidCache = -1;
  _captive = false;
  _updated = false;
  _freightCache = 0;

  _custCurrency->setLabel(_custCurrencyLit);

  _freight->clear();
  _payment->clear();

  _soitem->addColumn(tr("#"),          _seqColumn,   Qt::AlignCenter, true, "linenumber" );
  _soitem->addColumn(tr("Item"),       -1,           Qt::AlignLeft  , true, "item_number" );
  _soitem->addColumn(tr("Site"),       _whsColumn,   Qt::AlignCenter, true, "warehous_code" );
  _soitem->addColumn(tr("UOM"),        _uomColumn,   Qt::AlignLeft  , true, "uom_name" );
  _soitem->addColumn(tr("Ordered"),    _qtyColumn,   Qt::AlignRight , true, "coitem_qtyord" );
  _soitem->addColumn(tr("Shipped"),    _qtyColumn,   Qt::AlignRight , true, "coitem_qtyshipped" );
  _soitem->addColumn(tr("Returned"),   _qtyColumn,   Qt::AlignRight , true, "coitem_qtyreturned" );
  _soitem->addColumn(tr("Uninvoiced"), _qtyColumn,   Qt::AlignRight , true, "qtyatship" );
  _soitem->addColumn(tr("Approved"),   _qtyColumn,   Qt::AlignRight , true, "qtytobill" );
  _soitem->addColumn(tr("Extended"),   _moneyColumn, Qt::AlignRight , true, "extended" );
  _soitem->addColumn(tr("Close"),      _ynColumn,    Qt::AlignCenter, true, "toclose");

  if (_privileges->check("MaintainSalesOrders"))
    connect(_so, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));

  if(!_privileges->check("AllowSelectOrderEditing"))
  {
    _miscCharge->setEnabled(false);
    _miscChargeDescription->setEnabled(false);
    _miscChargeAccount->setEnabled(false);
  }
  else
  {
    connect(_soitem, SIGNAL(valid(bool)), _select, SLOT(setEnabled(bool)));
    connect(_soitem, SIGNAL(valid(bool)), _selectBalance, SLOT(setEnabled(bool)));
    connect(_soitem, SIGNAL(valid(bool)), _cancel, SLOT(setEnabled(bool)));
  }

  // some customers are creating scripts to show these widgets, probably shouldn't remove
  _paymentLit->hide();
  _payment->hide(); // Issue 10254:  if no objections over time, we should ultimately remove this.

  _miscChargeAccount->setType(GLCluster::cRevenue | GLCluster::cExpense);

  _freightTaxtype->setCode("Freight");
  _miscChargeTaxtype->setCode("Misc");

  if (_metrics->value("TaxService") != "N")
  {
    _taxzoneLit->hide();
    _taxZone->hide();
  }
  else
  {
    _taxExemptLit->hide();
    _taxExempt->hide();
  }
}

selectOrderForBilling::~selectOrderForBilling()
{
  // no need to delete child widgets, Qt does it all for us
}

void selectOrderForBilling::languageChange()
{
  retranslateUi(this);
}

SetResponse selectOrderForBilling::set(const ParameterList &pParams)
{
  XWidget::set(pParams);
  QVariant param;
  bool     valid;

  param = pParams.value("sohead_id", &valid);
  if (valid)
  {
    _captive = true;

    _so->setId(param.toInt());
  }

  param = pParams.value("mode", &valid);
  if (valid)
  {
    if (param.toString() == "new")
    {
      _mode = cNew;
      _salesTax->setMode(_mode);
    }
    else if (param.toString() == "edit")
    {
      _mode = cEdit;
      _salesTax->setMode(_mode);

      _close->setText(tr("&Cancel"));

    }
  }

  return NoError;
}

void selectOrderForBilling::clear()
{
  _so->setId(-1);
  _cobmiscid = -1;
  _salesTax->setOrderId(_cobmiscid);
  _orderDate->clear();
  _shipDate->clear();
  _invoiceDate->clear();
  _custName->clear();
  _poNumber->clear();
  _shipToName->clear();
  _custCurrency->setCurrentIndex(0);
  _taxZone->setId(-1);
  _soitem->clear();
  _shipvia->clear();
  _miscChargeAccount->setId(-1);
  _miscChargeDescription->clear();
  _comments->clear();
  _subtotal->clear();
  _miscCharge->clear();
  _freight->clear();
  _salesTax->clear();
  _payment->clear();
  _total->clear();
}

void selectOrderForBilling::sSave()
{
  save(false);

  omfgThis->sBillingSelectionUpdated(_so->id(), true);

  if (_metrics->value("TaxService") == "A")
    _salesTax->save();

  if (_captive)
    close();
  else
  {
    clear();
    _so->setFocus();
  }
}

bool selectOrderForBilling::save(bool partial)
{
  XSqlQuery selectSave;

  QList<GuiErrorCheck> errors;
  errors << GuiErrorCheck(!_shipDate->isValid(), _shipDate,
                          tr("<p>You must enter a Ship Date before "
                             "approving this order for billing."))
         << GuiErrorCheck(!_miscCharge->isZero() && !_miscChargeAccount->isValid(), _miscChargeAccount,
                          tr("<p>You may not enter a Misc. Charge without "
                             "indicating the G/L Sales Account number for the "
                             "charge.  Please set the Misc. Charge amount to 0 "
                             "or select a Misc. Charge Sales Account."))
         << GuiErrorCheck(_miscCharge->isZero() && _miscChargeAccount->isValid(), _miscCharge,
                          tr("<p>You must enter a Misc. Charge when "
                             "specifying a Misc. Charge Sales Account. "
                             "Please enter Misc. Charge amount "
                             "or remove the Misc. Charge Sales Account."))
         << GuiErrorCheck(_total->localValue() < 0, _miscCharge,
                          tr("<p>You may not approve "
			     "for billing a negative total amount"));

  if (partial && GuiErrorCheck::checkForErrors(errors))
    return false;

  if (GuiErrorCheck::reportErrors(this, tr("Cannot Approve for Billing"), errors))
      return false;

  if (_cobmiscid != -1)
  {
    selectSave.prepare( "UPDATE cobmisc "
               "SET cobmisc_freight=:cobmisc_freight,"
               "    cobmisc_payment=:cobmisc_payment,"
	       "    cobmisc_taxzone_id=:cobmisc_taxzone_id,"
	       "    cobmisc_notes=:cobmisc_notes,"
               "    cobmisc_shipdate=:cobmisc_shipdate, cobmisc_invcdate=:cobmisc_invcdate,"
               "    cobmisc_shipvia=:cobmisc_shipvia, cobmisc_closeorder=:cobmisc_closeorder,"
               "    cobmisc_misc=:cobmisc_misc, cobmisc_misc_accnt_id=:cobmisc_misc_accnt_id,"
               "    cobmisc_misc_descrip=:cobmisc_misc_descrip, "
	       "    cobmisc_curr_id=:cobmisc_curr_id, "
               "    cobmisc_freight_taxtype_id=:cobmisc_freight_taxtype_id, "
               "    cobmisc_misc_taxtype_id=:cobmisc_misc_taxtype_id, "
               "    cobmisc_misc_discount=:cobmisc_misc_discount, "
               "    cobmisc_tax_exemption=:cobmisc_tax_exemption "
               "WHERE (cobmisc_id=:cobmisc_id);" );
    selectSave.bindValue(":cobmisc_id", _cobmiscid);
    selectSave.bindValue(":cobmisc_freight", _freight->localValue());
    selectSave.bindValue(":cobmisc_payment", _payment->localValue());

    if (_taxZone->isValid())
      selectSave.bindValue(":cobmisc_taxzone_id", _taxZone->id());

    selectSave.bindValue(":cobmisc_notes", _comments->toPlainText());
    selectSave.bindValue(":cobmisc_shipdate", _shipDate->date());
    selectSave.bindValue(":cobmisc_invcdate", _invoiceDate->date());
    selectSave.bindValue(":cobmisc_shipvia", _shipvia->text());
    selectSave.bindValue(":cobmisc_closeorder", QVariant(_closeOpenItems->isChecked()));
    selectSave.bindValue(":cobmisc_misc", _miscCharge->localValue());
    selectSave.bindValue(":cobmisc_misc_accnt_id", _miscChargeAccount->id());
    selectSave.bindValue(":cobmisc_misc_descrip", _miscChargeDescription->text().trimmed());
    selectSave.bindValue(":cobmisc_curr_id",	_custCurrency->id());
    selectSave.bindValue(":cobmisc_freight_taxtype_id", _freightTaxtype->id());
    selectSave.bindValue(":cobmisc_misc_taxtype_id", _miscChargeTaxtype->id());
    selectSave.bindValue(":cobmisc_misc_discount", _miscChargeDiscount->isChecked());
    selectSave.bindValue(":cobmisc_tax_exemption", _taxExempt->code());
    selectSave.exec();
    if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Selected Orders For Billing"),
                                  selectSave, __FILE__, __LINE__))
    {
      return false;
    }
  }

  return true;
}

void selectOrderForBilling::sSoList()
{
  ParameterList params;
  params.append("sohead_id", _so->id());

  if (_showClosed->isChecked())
    params.append("soType", (cSoOpen | cSoClosed));
  else
    params.append("soType", cSoOpen);

  salesOrderList newdlg(this, "", true);
  newdlg.set(params);

  int id = newdlg.exec();
  if(id != QDialog::Rejected)
    _so->setId(id);
}

void selectOrderForBilling::sPopulate(int pSoheadid)
{
  XSqlQuery selectPopulate;
  if (_so->isValid())
  {
    selectPopulate.prepare("SELECT createBillingHeader(:sohead_id) AS cobmisc_id;");
    selectPopulate.bindValue(":sohead_id", pSoheadid);
    selectPopulate.exec();
    if (selectPopulate.first())
    {
      _cobmiscid = selectPopulate.value("cobmisc_id").toInt();
      _salesTax->setOrderId(_cobmiscid);
      if (_cobmiscid < 0)
      {
        ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Sales Order Information"),
                               storedProcErrorLookup("createBillingHeader", _cobmiscid),
                               __FILE__, __LINE__);
        return;
      }
    }
    else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Sales Order Information"),
                                  selectPopulate, __FILE__, __LINE__))
    {
      return;
    }

    XSqlQuery cobmisc;
    cobmisc.prepare( "SELECT cobmisc_id, cobmisc_notes, cobmisc_shipvia,"
                     "       cohead_orderdate, cobmisc_shipdate,"
		     "       cobmisc_invcdate, cobmisc_taxzone_id,"
                     "       cobmisc_freight AS freight,"
                     "       cobmisc_misc, cobmisc_misc_accnt_id, cobmisc_misc_descrip,"
                     "       cobmisc_payment AS payment,"
                     "       cobmisc_closeorder,"
                     "       cobmisc_curr_id,"
                     "       cobmisc_freight_taxtype_id, cobmisc_misc_taxtype_id,"
                     "       cobmisc_misc_discount,"
                     "       cobmisc_tax_exemption,"
                     "       cohead_number, cohead_shipto_id,"
                     "       cohead_custponumber,"
                     "       cohead_billtoname, cohead_shiptoname,"
                     "       CASE WHEN (shipchrg_custfreight IS NULL) THEN true"
                     "            ELSE shipchrg_custfreight"
                     "       END AS custfreight "
                     "FROM cobmisc , cohead "
                     "  LEFT OUTER JOIN shipchrg ON (cohead_shipchrg_id=shipchrg_id) "
                     "WHERE ( (cobmisc_id=:cobmisc_id)"
                     " AND (cohead_id=:cohead_id) ) ");
    cobmisc.bindValue(":cobmisc_id", _cobmiscid);
    cobmisc.bindValue(":cohead_id", pSoheadid);
    cobmisc.exec();
    if (cobmisc.first())
    {
      _cobmiscid = cobmisc.value("cobmisc_id").toInt();
      _salesTax->setOrderId(_cobmiscid);
      // do taxzone first so we can overwrite the result of the signal cascade
      _taxzoneidCache = cobmisc.value("cobmisc_taxzone_id").toInt();
      _taxZone->setId(cobmisc.value("cobmisc_taxzone_id").toInt());

      _orderDate->setDate(cobmisc.value("cohead_orderdate").toDate(), true);
      _shipDate->setDate(cobmisc.value("cobmisc_shipdate").toDate());
      _invoiceDate->setDate(cobmisc.value("cobmisc_invcdate").toDate());

      _poNumber->setText(cobmisc.value("cohead_custponumber").toString());
      _custName->setText(cobmisc.value("cohead_billtoname").toString());
      _shipToName->setText(cobmisc.value("cohead_shiptoname").toString());
      _shipvia->setText(cobmisc.value("cobmisc_shipvia").toString());
     
      _miscCharge->setLocalValue(cobmisc.value("cobmisc_misc").toDouble());
      _miscChargeAccount->setId(cobmisc.value("cobmisc_misc_accnt_id").toInt());
      _miscChargeDescription->setText(cobmisc.value("cobmisc_misc_descrip"));
      _miscChargeTaxtype->setId(cobmisc.value("cobmisc_misc_taxtype_id").toInt());
      _miscChargeDiscount->setChecked(cobmisc.value("cobmisc_misc_discount").toBool());
      _freightTaxtype->setId(cobmisc.value("cobmisc_freight_taxtype_id").toInt());
      _taxExempt->setCode(cobmisc.value("cobmisc_tax_exemption").toString());
      _payment->set(cobmisc.value("payment").toDouble(),
		    cobmisc.value("cobmisc_curr_id").toInt(),
		    cobmisc.value("cohead_orderdate").toDate(), false);
      _custCurrency->setId(cobmisc.value("cobmisc_curr_id").toInt());
      _comments->setText(cobmisc.value("cobmisc_notes").toString());
      _closeOpenItems->setChecked(cobmisc.value("cobmisc_closeorder").toBool());
      if (cobmisc.value("custfreight").toBool())
      {
        _freight->setEnabled(true);
        _freight->setLocalValue(cobmisc.value("freight").toDouble());
      }
      else
      {
        _freight->setEnabled(false);
        _freight->clear();
      }
      _freightCache = cobmisc.value("freight").toDouble();
    }
    else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Billing Selection Information"),
                                  selectPopulate, __FILE__, __LINE__))
    {
      return;
    }
  }
  else
  {
    clear();
  }

  sFillList();
}

void selectOrderForBilling::sEditOrder()
{
  salesOrder::editSalesOrder(_so->id(), false);
}

void selectOrderForBilling::sEditSelection()
{
  ParameterList params;
  params.append("soitem_id", _soitem->id());
  params.append("taxzone_id", _taxZone->id());

  selectBillingQty newdlg(this, "", true);
  newdlg.set(params);

  if (newdlg.exec() != XDialog::Rejected)
  {
    sFillList();

    _salesTax->invalidate();

    _updated = true;
  }
}

void selectOrderForBilling::sCancelSelection()
{
  XSqlQuery selectCancelSelection;
  selectCancelSelection.prepare( "DELETE FROM cobill "
             "WHERE ((cobill_coitem_id=:coitem_id)"
             " AND (SELECT NOT cobmisc_posted"
             "      FROM cobmisc"
             "      WHERE (cobill_cobmisc_id=cobmisc_id) ) ) ");
  selectCancelSelection.bindValue(":coitem_id", _soitem->id());
  selectCancelSelection.exec();

  sFillList();

  _salesTax->invalidate();
}

void selectOrderForBilling::sSelectBalance()
{
  XSqlQuery selectSelectBalance;
  selectSelectBalance.prepare("SELECT selectBalanceForBilling(:sohead_id) AS result;");
  selectSelectBalance.bindValue(":sohead_id", _so->id());
  selectSelectBalance.exec();
  if (selectSelectBalance.first())
  {
    int result = selectSelectBalance.value("result").toInt();
    if (result < 0)
    {
      ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Billing Information"),
                             storedProcErrorLookup("selectBalanceForBilling", result),
                             __FILE__, __LINE__);
      return;
    }
  }
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Billing Information"),
                                selectSelectBalance, __FILE__, __LINE__))
  {
    return;
  }

  sFillList();

  _salesTax->invalidate();
}

void selectOrderForBilling::sUpdateTotal()
{
  _total->setLocalValue( _subtotal->localValue() +
                                _salesTax->localValue() +
                                _miscCharge->localValue() +
                                _freight->localValue() );
}

void selectOrderForBilling::sFillList()
{
  XSqlQuery selectFillList;
  _soitem->clear();

  if (_so->isValid())
  {
    QString sql( "SELECT coitem_id, formatSoLineNumber(coitem_id) AS linenumber,"
                 "       item_number, iteminvpricerat(item_id) AS invpricerat,"
                 "       warehous_code, coitem_price,"
                 "       uom_name, coitem_qtyord, coitem_qtyshipped,"
                 "       coitem_qtyreturned,"
                 "       ( SELECT COALESCE(SUM(shipitem_qty), 0)"
                 "         FROM shiphead JOIN shipitem ON (shipitem_shiphead_id=shiphead_id) "
                 "         WHERE ( (shipitem_orderitem_id=coitem_id)"
                 "          AND (NOT shipitem_invoiced) "
                 "          AND (shiphead_order_type='SO') "
                 "          AND (shiphead_shipped) ) ) AS qtyatship,"
                 "       ( SELECT COALESCE(SUM(cobill_qty), 0)"
                 "         FROM cobill, cobmisc "
                 "         WHERE ( (cobill_cobmisc_id=cobmisc_id)"
                 "          AND (cobill_coitem_id=coitem_id)"
                 "          AND (NOT cobmisc_posted) ) ) AS qtytobill,"
                 "       round(( "
                 "         ( SELECT COALESCE(SUM(cobill_qty), 0)"
                 "           FROM cobill, cobmisc "
                 "           WHERE ( (cobill_cobmisc_id=cobmisc_id)"
                 "            AND (cobill_coitem_id=coitem_id)"
                 "            AND (NOT cobmisc_posted) ) ) * coitem_qty_invuomratio "
                 "           * ( coitem_price / coitem_price_invuomratio)), 2) AS extended, "
                 "       COALESCE((SELECT cobill_toclose"
                 "                       FROM cobill, cobmisc "
                 "                       WHERE ((cobill_cobmisc_id=cobmisc_id)"
                 "                        AND (cobill_coitem_id=coitem_id)"
                 "                        AND (NOT cobmisc_posted))"
                 "                       ORDER BY cobill_toclose DESC"
                 "                       LIMIT 1),false) AS toclose, "
                 "       'qty' AS coitem_qtyord_xtnumericrole, "
                 "       'qty' AS coitem_qtyshipped_xtnumericrole, "
                 "       'qty' AS coitem_qtyreturned_xtnumericrole, "
                 "       'qty' AS qtyatship_xtnumericrole, "
                 "       'qty' AS qtytobill_xtnumericrole, "
                 "       'extprice' AS extended_xtnumericrole "
                 "FROM coitem, itemsite, item, site(), uom "
                 "WHERE ( (coitem_itemsite_id=itemsite_id)"
                 " AND (coitem_status <> 'X')"
                 " AND (coitem_qty_uom_id=uom_id)"
                 " AND (itemsite_item_id=item_id)"
                 " AND (itemsite_warehous_id=warehous_id)"
		 " <? if exists(\"showOpenOnly\") ?>"
		 " AND (coitem_status <> 'C')"
		 " <? endif ?>"
		 " AND (coitem_cohead_id=<? value(\"sohead_id\") ?>) ) "
		 "ORDER BY coitem_linenumber, coitem_subnumber;" );

    ParameterList params;
    if (!_showClosed->isChecked())
      params.append("showOpenOnly");
    params.append("sohead_id", _so->id());
    MetaSQLQuery mql(sql);
    selectFillList = mql.toQuery(params);
    _soitem->populate(selectFillList);
    if (selectFillList.first())
    {
      double subtotal = 0.0;
      do
        subtotal += selectFillList.value("extended").toDouble();
      while (selectFillList.next());
      _subtotal->setLocalValue(subtotal);
    }
    else
    {
      if (selectFillList.lastError().type() != QSqlError::NoError)
      ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Billing Information"),
                           selectFillList, __FILE__, __LINE__);
      _subtotal->clear();
    }
  }
}

void selectOrderForBilling::sHandleShipchrg(int pShipchrgid)
{
  XSqlQuery query;
  query.prepare( "SELECT shipchrg_custfreight "
                 "FROM shipchrg "
                 "WHERE (shipchrg_id=:shipchrg_id);" );
  query.bindValue(":shipchrg_id", pShipchrgid);
  query.exec();
  if (query.first())
  {
    if (query.value("shipchrg_custfreight").toBool())
      _freight->setEnabled(true);
    else
    {
      _freight->setEnabled(false);
      _freight->clear();
    }
  }
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Shipping Charge Information"),
                                query, __FILE__, __LINE__))
  {
    return;
  }
}

void selectOrderForBilling::closeEvent(QCloseEvent * pEvent)
{
  XSqlQuery selectcloseEvent;
  selectcloseEvent.prepare("SELECT releaseUnusedBillingHeader(:cobmisc_id) AS result;");
  selectcloseEvent.bindValue(":cobmisc_id", _cobmiscid);
  selectcloseEvent.exec();
  if (selectcloseEvent.first())
  {
    int result = selectcloseEvent.value("result").toInt();
    if (result < -2) // don't bother the user with -1:posted or -2:has-lines
      ErrorReporter::error(QtCriticalMsg, this, tr("Error Releasing Unused Billing Header"),
                             storedProcErrorLookup("releaseUnusedBillingHeader", result),
                             __FILE__, __LINE__);
  }
  else if (selectcloseEvent.lastError().type() != QSqlError::NoError)
    ErrorReporter::error(QtCriticalMsg, this, tr("Error Releasing Unused Billing Header"),
                       selectcloseEvent, __FILE__, __LINE__);

  XWidget::closeEvent(pEvent);
}

void selectOrderForBilling::sTaxZoneChanged()
{
  if (_cobmiscid != -1 && _taxzoneidCache != _taxZone->id())
  {
    XSqlQuery taxq;
    taxq.prepare("UPDATE cobmisc SET "
      "  cobmisc_taxzone_id=:taxzone_id "
      "WHERE (cobmisc_id=:cobmisc_id) ");
    if (_taxZone->id() != -1)
      taxq.bindValue(":taxzone_id", _taxZone->id());
    taxq.bindValue(":cobmisc_id", _cobmiscid);
    taxq.bindValue(":freight", _freight->localValue());
    taxq.exec();
    if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Updating Tax Information"),
                                  taxq, __FILE__, __LINE__))
    {
      return;
    }
    _taxzoneidCache = _taxZone->id();
    if (_metrics->value("TaxService") == "N")
      _salesTax->sRecalculate();
  }
}

void selectOrderForBilling::sMiscTaxtypeChanged()
{
  _salesTax->invalidate();
}

void selectOrderForBilling::sMiscChargeChanged()
{
  sUpdateTotal();

  _salesTax->invalidate();
}

void selectOrderForBilling::sFreightChanged()
{
  if (_cobmiscid != -1 && _freightCache != _freight->localValue())
  {
    if (_metrics->value("TaxService") == "A")
    {
      XSqlQuery qry;
      qry.prepare("SELECT COUNT(DISTINCT ARRAY[]::TEXT[] || "
                  "                      addr_line1 || addr_line2 || addr_line3 || "
                  "                      addr_city || addr_state || addr_postalcode || "
                  "                      addr_country) > 1 "
                  "       AS check "
                  "  FROM cobill "
                  "  JOIN coitem ON cobill_coitem_id = coitem_id "
                  "  JOIN itemsite ON coitem_itemsite_id = itemsite_id "
                  "  JOIN whsinfo ON itemsite_warehous_id = warehous_id "
                  "  LEFT OUTER JOIN addr ON warehous_addr_id = addr_id "
                  " WHERE cobill_cobmisc_id = :cobmiscid;");
      qry.bindValue(":cobmiscid", _cobmiscid);
      qry.exec();
      if (qry.first() && qry.value("check").toBool())
      {
        QMessageBox::critical(this, tr("Cannot override freight"),
                              tr("Freight must be calculated automatically when lines are "
                                 "shipping from different addresses for Avalara tax "
                                 "calculation."));

        disconnect(_freight, SIGNAL(valueChanged()), this, SLOT(sFreightChanged()));
        _freight->setLocalValue(_freightCache);
        connect(_freight, SIGNAL(valueChanged()), this, SLOT(sFreightChanged()));

        return;
      }
      else if (ErrorReporter::error(QtCriticalMsg, this, tr("Failed to check freight"),
                                    qry, __FILE__, __LINE__))
      {
        disconnect(_freight, SIGNAL(valueChanged()), this, SLOT(sFreightChanged()));
        _freight->setLocalValue(_freightCache);
        connect(_freight, SIGNAL(valueChanged()), this, SLOT(sFreightChanged()));

        return;
      }
    }

    XSqlQuery taxq;
    taxq.prepare("UPDATE cobmisc SET "
      "  cobmisc_freight=:freight "
      "WHERE (cobmisc_id=:cobmisc_id) ");
    taxq.bindValue(":cobmisc_id", _cobmiscid);
    taxq.bindValue(":freight", _freight->localValue());
    taxq.exec();
    if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Updating Freight Information"),
                                          taxq, __FILE__, __LINE__))
    {
      return;
    }
    _freightCache = _freight->localValue();

    sUpdateTotal();

    _salesTax->invalidate();
  }   
}
