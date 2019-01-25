/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "xclusterinputdialog.h"

#include <QMessageBox>
#include <QValidator>
#include <QVariant>

#include "addresscluster.h"
#include "crmacctcluster.h"

XClusterInputDialog::XClusterInputDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : XDialog(parent, name, modal, fl)
{
    setupUi(this);
}

XClusterInputDialog::~XClusterInputDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void XClusterInputDialog::languageChange()
{
    retranslateUi(this);
}

enum SetResponse XClusterInputDialog::set(const ParameterList &pParams)
{
  XDialog::set(pParams);
  QVariant param;
  bool     valid;

  param = pParams.value("type", &valid);
  if (valid)
  {
    if (param.toString() == "crmacct")
    {
      _cluster = new CRMAcctCluster(this, "_cluster");
      this->findChildren<QHBoxLayout*>()[0]->insertWidget(0, _cluster);
    } 
    else if (param.toString() == "addr")
    {
      _cluster = new AddressCluster(this, "_cluster");
      this->findChildren<QHBoxLayout*>()[0]->insertWidget(0, _cluster);
    } 
  }

  param = pParams.value("label", &valid);
  if (valid)
    _cluster->setLabel(param.toString());

  param = pParams.value("default", &valid);
  if (valid)
  {
    _cluster->setId(param.toInt());
  }

  return NoError;
}

int XClusterInputDialog::getId()
{
  return _cluster->id();
}
