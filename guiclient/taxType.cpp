/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxType.h"

#include <QVariant>
#include <QMessageBox>

#include "errorReporter.h"
#include "guiErrorCheck.h"
#include "mqlutil.h"
#include "taxIntegration.h"


taxType::taxType(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
  : XDialog(parent, name, modal, fl)
{
  setupUi(this);

  connect(_save, SIGNAL(clicked()), this, SLOT(sSave()));
  connect(_close, SIGNAL(clicked()), this, SLOT(reject()));
  connect(_name, SIGNAL(editingFinished()), this, SLOT(sCheck()));
  connect(_externalCodeList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(sUpdateExtTaxCode()));
  connect(_search, SIGNAL(editingFinished()), this, SLOT(populateServiceList()));

  if (_metrics->value("TaxService") == "A")
  {
    _extService = true;
    _externalCodeList->addColumn(tr("Code"),        -1, Qt::AlignLeft, true, "taxcode" );
    _externalCodeList->addColumn(tr("Description"), -1, Qt::AlignLeft, true, "description" );
  }
  else
  {
    _extService = false;
    _externalCode->setVisible(false);
    _externalCodeLit->setVisible(false);
    _externalCodeList->setVisible(false);
    _externalCodeListLit->setVisible(false);
    _searchLit->setVisible(false);
    _search->setVisible(false);
  }
}

taxType::~taxType()
{
  // no need to delete child widgets, Qt does it all for us
}

void taxType::languageChange()
{
  retranslateUi(this);
}

enum SetResponse taxType::set(const ParameterList &pParams)
{
  XDialog::set(pParams);
  QVariant param;
  bool     valid;

  param = pParams.value("taxtype_id", &valid);
  if (valid)
  {
    _taxtypeid = param.toInt();
    populate();
  }

  param = pParams.value("mode", &valid);
  if (valid)
  {
    if (param.toString() == "new")
    {
      _mode = cNew;
    }
    else if (param.toString() == "edit")
    {
      _mode = cEdit;
    }
    else if (param.toString() == "view")
    {
      _mode = cView;

      _name->setEnabled(false);
      _description->setEnabled(false);
      _close->setText(tr("&Close"));
      _save->hide();

      _externalCodeList->setVisible(false);
      _externalCodeListLit->setVisible(false);
      _searchLit->setVisible(false);
      _search->setVisible(false);
    }
  }

  if (_extService && _mode != cView)
    populateServiceList();

  return NoError;
}

void taxType::sCheck()
{
  XSqlQuery taxCheck;
  _name->setText(_name->text().trimmed());
  if ( (_mode == cNew) && (_name->text().length()) )
  {
    taxCheck.prepare( "SELECT taxtype_id "
               "FROM taxtype "
               "WHERE (UPPER(taxtype_name)=UPPER(:taxtype_name));" );
    taxCheck.bindValue(":taxtype_name", _name->text());
    taxCheck.exec();
    if (taxCheck.first())
    {
      _taxtypeid = taxCheck.value("taxtype_id").toInt();
      _mode = cEdit;
      populate();

      _name->setEnabled(false);
    }
  }
}

void taxType::sSave()
{
  XSqlQuery taxSave;

  QList<GuiErrorCheck> errors;
    errors<< GuiErrorCheck(_name->text().trimmed().isEmpty(), _name,
                           tr("You must name this Tax Type before saving it."))
          << GuiErrorCheck(_externalCode->text().trimmed().isEmpty() && _extService, _externalCodeList,
                           tr("You must define an external tax code when using an external taxation service."))
    ;
    if (GuiErrorCheck::reportErrors(this, tr("Missing Name"), errors))
      return;

  if (_mode == cEdit)
  {
    taxSave.prepare( "SELECT taxtype_id "
               "FROM taxtype "
               "WHERE ( (taxtype_id<>:taxtype_id)"
               " AND (UPPER(taxtype_name)=UPPER(:taxtype_name)) );");
    taxSave.bindValue(":taxtype_id", _taxtypeid);
    taxSave.bindValue(":taxtype_name", _name->text().trimmed());
    taxSave.exec();

    QList<GuiErrorCheck> errors;
    errors<< GuiErrorCheck(taxSave.first(), _name,
                           tr("A Tax Type with the entered name already exists."
                                 "You may not create a Tax Type with this name."))
    ;
    if (GuiErrorCheck::reportErrors(this, tr("Cannot Create Tax Type"), errors))
      return;

    taxSave.prepare( "UPDATE taxtype "
               "SET taxtype_name=:taxtype_name,"
               "    taxtype_descrip=:taxtype_descrip, "
               "    taxtype_external_code=:taxtype_external_code "
               "WHERE (taxtype_id=:taxtype_id);" );
    taxSave.bindValue(":taxtype_id", _taxtypeid);
    taxSave.bindValue(":taxtype_name", _name->text().trimmed());
    taxSave.bindValue(":taxtype_descrip", _description->text());
    taxSave.bindValue(":taxtype_external_code", _externalCode->text());
    taxSave.exec();
  }
  else if (_mode == cNew)
  {
    taxSave.prepare( "SELECT taxtype_id "
               "FROM taxtype "
               "WHERE (UPPER(taxtype_name)=UPPER(:taxtype_name));");
    taxSave.bindValue(":taxtype_name", _name->text().trimmed());
    taxSave.exec();

    QList<GuiErrorCheck> errors;
    errors<< GuiErrorCheck(taxSave.first(), _name,
                           tr("A Tax Type with the entered name already exists."
                                 "You may not create a Tax Type with this name."))
    ;
    if (GuiErrorCheck::reportErrors(this, tr("Cannot Create Tax Type"), errors))
      return;

    taxSave.exec("SELECT NEXTVAL('taxtype_taxtype_id_seq') AS taxtype_id;");
    if (taxSave.first())
      _taxtypeid = taxSave.value("taxtype_id").toInt();
    else
    {
      ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Tax Type Information"),
                           taxSave, __FILE__, __LINE__);
      return;
    }

    taxSave.prepare( "INSERT INTO taxtype "
               "( taxtype_id, taxtype_name, taxtype_descrip, taxtype_external_code ) "
               "VALUES "
               "( :taxtype_id, :taxtype_name, :taxtype_descrip, :taxtype_external_code );" );
    taxSave.bindValue(":taxtype_id", _taxtypeid);
    taxSave.bindValue(":taxtype_name", _name->text().trimmed());
    taxSave.bindValue(":taxtype_descrip", _description->text());
    taxSave.bindValue(":taxtype_external_code", _externalCode->text());
    taxSave.exec();
  }

  done(_taxtypeid);
}

void taxType::populate()
{
  XSqlQuery taxpopulate;
  taxpopulate.prepare( "SELECT taxtype_name, taxtype_descrip, taxtype_sys, taxtype_external_code "
             "FROM taxtype "
             "WHERE (taxtype_id=:taxtype_id);" );
  taxpopulate.bindValue(":taxtype_id", _taxtypeid);
  taxpopulate.exec();
  if (taxpopulate.first())
  {
    _name->setText(taxpopulate.value("taxtype_name").toString());
    if(taxpopulate.value("taxtype_sys").toBool())
      _name->setEnabled(false);
    _description->setText(taxpopulate.value("taxtype_descrip").toString());
    _externalCode->setText(taxpopulate.value("taxtype_external_code").toString());
  }
}

void taxType::populateServiceList()
{
  XSqlQuery qry;
  QJsonObject taxCodes;

  TaxIntegration* tax = TaxIntegration::getTaxIntegration();
  taxCodes = tax->getTaxCodes();
  QString sql("SELECT * FROM formatExternalTaxCodes(<? value('taxCodes') ?>) "
              "<? if exists('search') ?>"
              "WHERE taxcode ~* <? value('search') ?>"
              "   OR description ~* <? value('search') ?>"
              "<? endif ?>"
              "ORDER BY taxcode;");
  MetaSQLQuery  mql(sql);
  ParameterList params;
  params.append("taxCodes", QString::fromUtf8(QJsonDocument(taxCodes).toJson()));
  if (!_search->text().trimmed().isEmpty())
    params.append("search", _search->text().trimmed());

  qry = mql.toQuery(params);
  _externalCodeList->populate(qry);
  ErrorReporter::error(QtCriticalMsg, this, tr("Error Retrieving Tax Type Information"),
                         qry, __FILE__, __LINE__);

  return;
}

void taxType::sUpdateExtTaxCode()
{
  QString sel = _externalCodeList->currentItem()->rawValue("taxcode").toString();
  if ((sel != _externalCode->text()) && _externalCode->text() != "")
  {
    if (QMessageBox::question(this, tr("Confirm Selection"),
                 tr("<p>You have selected a different external Tax Code for this type.<br>"
                    "Are you sure you wish to update the Tax Code?"),
                 QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
       return;
  }

  _externalCode->setText(sel);

}

