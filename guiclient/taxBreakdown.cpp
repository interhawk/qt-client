/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2014 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxBreakdown.h"

#include <QMessageBox>

#include "errorReporter.h"
#include "metasql.h"
#include "mqlutil.h"
#include "taxAdjustment.h"

taxBreakdown::taxBreakdown(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
  : XDialog(parent, name, modal, fl)
{
  setupUi(this);

  _currency->setLabel(_currencyLit);

  _orderid	= -1;
  _ordertype	= "";
  _sense        = 1;

  _tax->addColumn(tr("Line"), -1, Qt::AlignLeft, true, "line");
  _tax->addColumn(tr("Item"), -1, Qt::AlignLeft, true, "item_number");
  _tax->addColumn(tr("Amount"), -1, Qt::AlignLeft, true, "amount");
  _tax->addColumn(tr("Qty"), -1, Qt::AlignLeft, true, "qty");
  _tax->addColumn(tr("Extended"), -1, Qt::AlignLeft, true, "extended");
  _tax->addColumn(tr("Taxable Amount"), -1, Qt::AlignLeft, true, "taxhist_basis");
  _tax->addColumn(tr("Code"), -1, Qt::AlignLeft, true, "tax_code");
  _tax->addColumn(tr("Description"), -1, Qt::AlignLeft, true, "tax_descrip");
  _tax->addColumn(tr("Sequence"), -1, Qt::AlignLeft, true, "taxhist_sequence");
  _tax->addColumn(tr("Flat Amount"), -1, Qt::AlignLeft, true, "taxhist_amount");
  _tax->addColumn(tr("Percent"), -1, Qt::AlignLeft, true, "taxhist_percent");
  _tax->addColumn(tr("Tax"), -1, Qt::AlignLeft, true, "taxhist_tax");
  _tax->addColumn(tr("Total"), -1, Qt::AlignLeft, true, "total");

  connect(_tax, SIGNAL(valid(bool)), this, SLOT(sHandleButtons(bool)));
  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));

  if (_metrics->value("TaxService") != "N")
  {
    _new->hide();
    _delete->hide();
  }
}

taxBreakdown::~taxBreakdown()
{
  // no need to delete child widgets, Qt does it all for us
}

void taxBreakdown::languageChange()
{
  retranslateUi(this);
}

SetResponse taxBreakdown::set(const ParameterList& pParams)
{
  XDialog::set(pParams);
  QVariant param;
  bool	   valid;

  param = pParams.value("order_id", &valid);
  if (valid)
    _orderid = param.toInt();

  param = pParams.value("order_type", &valid);
  if (valid)
    _ordertype = param.toString();

  param = pParams.value("mode", &valid);
  if (valid)
  {
    if(param.toString() == "view")
      _mode = cView;
    else
      _mode = cEdit;
  }
  
 param = pParams.value("sense", &valid);
 if (valid)
  _sense = param.toInt();

  sPopulate();

  return NoError;
}

void taxBreakdown::sPopulate()
{
  XSqlQuery taxPopulate;
  ParameterList params;
  if (_ordertype == "S")
  {
    _currencyLit->setText(tr("Sales Order Currency:"));
    _header->setText(tr("Tax Breakdown for Sales Order:"));

    _new->hide();
    _delete->hide();

    taxPopulate.prepare("SELECT cohead_number AS number, cohead_taxzone_id AS taxzone_id, "
                        "       cohead_curr_id AS curr_id, cohead_orderdate AS date "
                        "  FROM cohead "
                        " WHERE cohead_id = :orderid ");

    params.append("headtype", "S");
    params.append("itemtype", "SI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "SI")
  {
    _currencyLit->setText(tr("Sales Order Currency:"));
    _header->setText(tr("Tax Breakdown for Sales Order:"));
 
    _new->hide();
    _delete->hide();

    taxPopulate.prepare("SELECT cohead_number AS number, cohead_taxzone_id AS taxzone_id, "
                        "       cohead_curr_id AS curr_id, cohead_orderdate AS date "
                        "  FROM coitem "
                        "  JOIN cohead ON coitem_cohead_id = cohead_id "
                        " WHERE coitem_id = :orderid ");

    params.append("headtype", "S");
    params.append("itemtype", "SI");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "Q")
  {
    _currencyLit->setText(tr("Quote Currency:"));
    _header->setText(tr("Tax Breakdown for Quote:"));
 
    _new->hide();
    _delete->hide();

    taxPopulate.prepare("SELECT quhead_number AS number, quhead_taxzone_id AS taxzone_id, "
                        "       quhead_curr_id AS curr_id, quhead_quotedate AS date "
                        "  FROM quhead "
                        " WHERE quhead_id = :orderid ");

    params.append("headtype", "Q");
    params.append("itemtype", "QI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "QI")
  {
    _currencyLit->setText(tr("Quote Currency:"));
    _header->setText(tr("Tax Breakdown for Quote:"));
 
    _new->hide();
    _delete->hide();

    taxPopulate.prepare("SELECT quhead_number AS number, quhead_taxzone_id AS taxzone_id, "
                        "       quhead_curr_id AS curr_id, quhead_quotedate AS date "
                        "  FROM quitem "
                        "  JOIN quhead ON quitem_quhead_id = quhead_id "
                        " WHERE quitem_id = :orderid ");

    params.append("headtype", "Q");
    params.append("itemtype", "QI");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "COB")
  {
    _currencyLit->setText(tr("Billing Currency:"));
    _header->setText(tr("Tax Breakdown for Billing Order:"));

    taxPopulate.prepare("SELECT cohead_number AS number, cobmisc_taxzone_id AS taxzone_id, "
                        "       cobmisc_curr_id AS curr_id, cobmisc_invcdate AS date "
                        "  FROM cobmisc "
                        "  JOIN cohead ON cobmisc_cohead_id = cohead_id "
                        " WHERE cobmisc_id = :orderid ");

    params.append("headtype", "COB");
    params.append("itemtype", "COBI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "INV")
  {
    _currencyLit->setText(tr("Invoice Currency:"));
    _header->setText(tr("Tax Breakdown for Invoice:"));

    taxPopulate.prepare("SELECT invchead_invcnumber AS number, invchead_taxzone_id AS taxzone_id, "
                        "       invchead_curr_id AS curr_id, invchead_invcdate AS date "
                        "  FROM invchead "
                        " WHERE invchead_id = :orderid ");

    params.append("headtype", "INV");
    params.append("itemtype", "INVI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "INVI")
  {
    _currencyLit->setText(tr("Invoice Currency:"));
    _header->setText(tr("Tax Breakdown for Invoice:"));

    taxPopulate.prepare("SELECT invchead_invcnumber AS number, invchead_taxzone_id AS taxzone_id, "
                        "       invchead_curr_id AS curr_id, invchead_invcdate AS date "
                        "  FROM invcitem "
                        "  JOIN invchead ON invcitem_invchead_id = invchead_id "
                        " WHERE invcitem_id = :orderid ");

    params.append("headtype", "INV");
    params.append("itemtype", "INVI");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "RA")
  {
    _currencyLit->setText(tr("Return Authorization Currency:"));
    _header->setText(tr("Tax Breakdown for Return:"));

    _new->hide();
    _delete->hide();

    taxPopulate.prepare("SELECT rahead_number AS number, rahead_taxzone_id AS taxzone_id, "
                        "       rahead_curr_id AS curr_id, rahead_authdate AS date "
                        "  FROM rahead "
                        " WHERE rahead_id = :orderid ");

    params.append("headtype", "RA");
    params.append("itemtype", "RI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "RI")
  {
    _currencyLit->setText(tr("Return Authorization Currency:"));
    _header->setText(tr("Tax Breakdown for Return:"));

    _new->hide();
    _delete->hide();

    taxPopulate.prepare("SELECT rahead_number AS number, rahead_taxzone_id AS taxzone_id, "
                        "       rahead_curr_id AS curr_id, rahead_authdate AS date "
                        "  FROM raitem "
                        "  JOIN rahead ON raitem_rahead_id = rahead_id "
                        " WHERE raitem_id = :orderid ");

    params.append("headtype", "RA");
    params.append("itemtype", "RI");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "CM")
  {
    _currencyLit->setText(tr("Sales Credit Currency:"));
    _header->setText(tr("Tax Breakdown for Sales Credit:"));

    taxPopulate.prepare("SELECT cmhead_number AS number, cmhead_taxzone_id AS taxzone_id, "
                        "       cmhead_curr_id AS curr_id, cmhead_docdate AS date "
                        "  FROM cmhead "
                        " WHERE cmhead_id = :orderid ");

    params.append("headtype", "CM");
    params.append("itemtype", "CMI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "CMI")
  {
    _currencyLit->setText(tr("Sales Credit Currency:"));
    _header->setText(tr("Tax Breakdown for Sales Credit:"));

    taxPopulate.prepare("SELECT cmhead_number AS number, cmhead_taxzone_id AS taxzone_id, "
                        "       cmhead_curr_id AS curr_id, cmhead_docdate AS date "
                        "  FROM cmitem "
                        "  JOIN cmhead ON cmitem_cmhead_id = cmhead_id "
                        " WHERE cmitem_id = :orderid ");

    params.append("headtype", "CM");
    params.append("itemtype", "CMI");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "TO")
  {
    _currencyLit->setText(tr("Transfer Order Currency:"));
    _header->setText(tr("Tax Breakdown for Transfer Order:"));

    taxPopulate.prepare("SELECT tohead_number AS number, tohead_taxzone_id AS taxzone_id, "
                        "       tohead_freight_curr_id AS curr_id, tohead_orderdate AS date "
                        "  FROM tohead "
                        " WHERE tohead_id = :orderid ");

    params.append("headtype", "TO");
    params.append("headtype", "TI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "TI")
  {
    _currencyLit->setText(tr("Transfer Order Currency:"));
    _header->setText(tr("Tax Breakdown for Transfer Order:"));

    taxPopulate.prepare("SELECT tohead_number AS number, tohead_taxzone_id AS taxzone_id, "
                        "       tohead_freight_curr_id AS curr_id, tohead_orderdate AS date "
                        "  FROM toitem "
                        "  JOIN tohead ON toitem_tohead_id = tohead_id "
                        " WHERE toitem_id = :orderid ");

    params.append("headtype", "TO");
    params.append("headtype", "TI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "P")
  {
    _currencyLit->setText(tr("Purchase Order Currency:"));
    _header->setText(tr("Tax Breakdown for Purchase Order:"));

    taxPopulate.prepare("SELECT pohead_number AS number, pohead_taxzone_id AS taxzone_id, "
                        "       pohead_curr_id AS curr_id, pohead_orderdate AS date "
                        "  FROM pohead "
                        " WHERE pohead_id = :orderid ");

    params.append("headtype", "P");
    params.append("itemtype", "PI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "PI")
  {
    _currencyLit->setText(tr("Purchase Order Currency:"));
    _header->setText(tr("Tax Breakdown for Purchase Order Item:"));

    taxPopulate.prepare("SELECT pohead_number AS number, pohead_taxzone_id AS taxzone_id, "
                        "       pohead_curr_id AS curr_id, pohead_orderdate AS date "
                        "  FROM poitem "
                        "  JOIN pohead ON poitem_pohead_id = pohead_id "
                        " WHERE poitem_id = :orderid ");

    params.append("headtype", "P");
    params.append("itemtype", "PI");
    params.append("docitem_id", _orderid);
  }
   else if (_ordertype == "VCH")
  {
    _currencyLit->setText(tr("Voucher Currency:"));
    _header->setText(tr("Tax Breakdown for Voucher:"));

    taxPopulate.prepare("SELECT vohead_number AS number, vohead_taxzone_id AS taxzone_id, "
                        "       vohead_curr_id AS curr_id, vohead_docdate AS date "
                        "  FROM vohead "
                        " WHERE vohead_id = :orderid ");

    params.append("headtype", "VCH");
    params.append("itemtype", "VCHI");
    params.append("dochead_id", _orderid);
  }
  else if (_ordertype == "VCHI")
  {
    _currencyLit->setText(tr("Voucher Currency:"));
    _header->setText(tr("Tax Breakdown for Voucher:"));

    taxPopulate.prepare("SELECT vohead_number AS number, vohead_taxzone_id AS taxzone_id, "
                        "       vohead_curr_id AS curr_id, vohead_docdate AS date "
                        "  FROM voitem "
                        "  JOIN vohead ON voitem_vohead_id = vohead_id "
                        " WHERE voitem_id = :orderid ");

    params.append("headtype", "VCH");
    params.append("itemtype", "VCHI");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "AR")
  {
    _currencyLit->setText(tr("A/R Open Currency:"));
    _header->setText(tr("Tax Breakdown for A/R Open:"));

    taxPopulate.prepare("SELECT aropen_docnumber AS number, aropen_taxzone_id AS taxzone_id, "
                        "       aropen_curr_id AS curr_id, aropen_docdate AS date "
                        "  FROM aropen "
                        " WHERE aropen_id = :orderid ");

    params.append("headtype", "AR");
    params.append("itemtype", "AR");
    params.append("docitem_id", _orderid);
  }
  else if (_ordertype == "AP")
  {
    _currencyLit->setText(tr("A/P Open Currency:"));
    _header->setText(tr("Tax Breakdown for A/P Open:"));

    taxPopulate.prepare("SELECT apopen_docnumber AS number, apopen_taxzone_id AS taxzone_id, "
                        "       apopen_curr_id AS curr_id, apopen_docdate AS date "
                        "  FROM apopen "
                        " WHERE apopen_id = :orderid ");

    params.append("headtype", "AP");
    params.append("itemtype", "AP");
    params.append("docitem_id", _orderid);
  }

  taxPopulate.bindValue(":orderid", _orderid);
  taxPopulate.exec();
  if (taxPopulate.first())
  {
    _document->setText(taxPopulate.value("number").toString());
    _taxzone->setId(taxPopulate.value("taxzone_id").toInt());
    _currency->setId(taxPopulate.value("curr_id").toInt());
    _date = taxPopulate.value("date").toDate();

    if (_date.isNull())
      _date = QDate::currentDate();
  }
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Tax Information"),
                                taxPopulate, __FILE__, __LINE__))
    return;

  MetaSQLQuery mql = mqlLoad("taxBreakdown", "detail");
  params.append("freight", tr("Freight"));
  params.append("group", tr("Group"));
  params.append("misc", tr("Misc"));
  params.append("adjustment", tr("Adjustment"));
  params.append("total", tr("Total"));

  XSqlQuery qry = mql.toQuery(params);
  if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Tax Information"),
                           qry, __FILE__, __LINE__))
    return;

  _tax->populate(qry, true);
}

void taxBreakdown::sHandleButtons(bool valid)
{
  _delete->setEnabled(valid && _tax->altId() == 3 &&
                      _tax->id() > 0);
}

void taxBreakdown::sNew()
{
  taxAdjustment newdlg(this, "", true);
  ParameterList params;
  params.append("order_id", _orderid);
  params.append("order_type", _ordertype);
  params.append("date", _date);
  params.append("curr_id", _currency->id());
  params.append("sense", _sense);
  params.append("mode", "new");
  newdlg.set(params);

  if (newdlg.exec() == XDialog::Accepted)
    sPopulate(); 
}

void taxBreakdown::sDelete()
{
  if (QMessageBox::question(this, tr("Delete Tax Adjustment?"),
                            tr("<p>Are you sure that you want to delete this tax adjustment?"),
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::No) == QMessageBox::Yes)
  {
    XSqlQuery taxDelete;
    taxDelete.prepare("DELETE FROM taxhist "
                      " WHERE taxhist_id = :taxhist_id;");
    taxDelete.bindValue(":taxhist_id", _tax->id());
    taxDelete.exec();

    if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Deleting Tax Adjustment Information"),
                             taxDelete, __FILE__, __LINE__))
      return;

    sPopulate();
  }
}
