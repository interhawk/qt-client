/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxAdjustment.h"

#include <QMessageBox>
#include <QSqlError>
#include <QVariant>

#include "storedProcErrorLookup.h"
#include "errorReporter.h"

taxAdjustment::taxAdjustment(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
  setupUi(this);

  connect(_save, SIGNAL(clicked()), this, SLOT(sSave()));
  connect(_taxcode, SIGNAL(newID(int)), this, SLOT(sCheck()));

  _taxdetailid = -1;
}

taxAdjustment::~taxAdjustment()
{
  // no need to delete child widgets, Qt does it all for us
}

void taxAdjustment::languageChange()
{
  retranslateUi(this);
}

void taxAdjustment::set(const ParameterList &pParams)
{
  QVariant param;
  bool     valid;

  param = pParams.value("order_id", &valid);
  if (valid)
    _orderid = param.toInt();

  param = pParams.value("order_type", &valid);
  if (valid)
    _ordertype = param.toString();

  param = pParams.value("mode", &valid);
  if (valid)
  {
    if (param.toString() == "new")
    {
      _mode = cNew;
    }
  }

  param = pParams.value("date", &valid);
   if (valid)
     _amount->setEffective(param.toDate());

  param = pParams.value("curr_id", &valid);
   if (valid)
     _amount->setId(param.toInt());
}

void taxAdjustment::sSave()
{
  XSqlQuery taxSave;
  if (_taxcode->id() == -1)
  {
    QMessageBox::critical( this, tr("Tax Code Required"),
                          tr( "<p>You must select a tax code "
                          " before you may save." ) );
    return;
  }

  if (_mode == cNew)
  {
    taxSave.prepare("INSERT INTO taxline (taxline_taxhead_id, taxline_line_type, "
                    "                     taxline_taxtype_id) "
                    "SELECT taxhead_id, 'A', "
                    "       getAdjustmentTaxtypeId() "
                    "  FROM taxhead "
                    " WHERE taxhead_doc_type = :ordertype "
                    "   AND taxhead_doc_id = :orderid "
                    "   AND NOT EXISTS (SELECT 1 "
                    "                     FROM taxline "
                    "                    WHERE taxline_taxhead_id = taxhead_id "
                    "                      AND taxline_line_type = 'A');");
    taxSave.bindValue(":ordertype", _ordertype);
    taxSave.bindValue(":orderid", _orderid);
    taxSave.exec();
    if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Tax Adjustment Information"),
                             taxSave, __FILE__, __LINE__))
      return;

    taxSave.prepare("INSERT INTO taxdetail (taxdetail_taxline_id, taxdetail_tax_id, taxdetail_tax) "
                    "SELECT taxline_id, :taxid, :tax "
                    "  FROM taxhead "
                    "  JOIN taxline ON taxhead_id = taxline_taxhead_id "
                    "              AND taxline_line_type = 'A' "
                    " WHERE taxhead_doc_type = :ordertype "
                    "   AND taxhead_doc_id = :orderid;");
    taxSave.bindValue(":ordertype", _ordertype);
    taxSave.bindValue(":orderid", _orderid);
    taxSave.bindValue(":taxid", _taxcode->id());
    taxSave.bindValue(":tax", _amount->localValue());
    taxSave.exec();
    ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Tax Adjustment Information"),
                         taxSave, __FILE__, __LINE__);
  }
  else if (_mode == cEdit)
  {
    taxSave.prepare("UPDATE taxdetail "
                    "   SET taxdetail_tax = :tax "
                    " WHERE taxdetail_id = :taxdetail_id;");
    taxSave.bindValue(":taxdetail_id", _taxdetailid);
    taxSave.exec();
    ErrorReporter::error(QtCriticalMsg, this, tr("Error Saving Tax Adjustment Information"),
                         taxSave, __FILE__, __LINE__);
  }

  accept();
}

void taxAdjustment::sCheck()
{
  XSqlQuery taxCheck;

  taxCheck.prepare("SELECT taxdetail_id, taxdetail_tax "
                   "  FROM taxhead "
                   "  JOIN taxline ON taxhead_id = taxline_taxhead_id "
                   "              AND taxline_line_type = 'A' "
                   "  JOIN taxdetail ON taxline_id = taxdetail_taxline_id "
                   "                AND taxdetail_tax_id = :taxid "
                   " WHERE taxhead_doc_type = :ordertype "
                   "   AND taxhead_doc_id = :orderid;");
  taxCheck.bindValue(":ordertype", _ordertype);
  taxCheck.bindValue(":orderid", _orderid);
  taxCheck.bindValue(":taxid", _taxcode->id());
  taxCheck.exec();
  if (taxCheck.first())
  {
    _taxdetailid = taxCheck.value("taxdetail_id").toInt();
    _amount->setLocalValue(taxCheck.value("taxdetail_tax").toDouble());
    _amount->setFocus();
    _mode = cEdit;
  }
  else
  {
    _taxdetailid = -1;
    _amount->clear();
    _amount->setFocus();
    _mode = cNew;
  }
}

