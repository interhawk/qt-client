/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "postVouchers.h"

#include <QMessageBox>
#include "guiErrorCheck.h"
#include <QSqlError>

#include <openreports.h>
#include "errorReporter.h"
#include "storedProcErrorLookup.h"

postVouchers::postVouchers(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : XDialog(parent, name, modal, fl)
{
  setupUi(this);

  connect(_post, SIGNAL(clicked()), this, SLOT(sPost()));

  if (_preferences->boolean("XCheckBox/forgetful"))
    _printJournal->setChecked(true);
}

postVouchers::~postVouchers()
{
  // no need to delete child widgets, Qt does it all for us
}

void postVouchers::languageChange()
{
    retranslateUi(this);
}

void postVouchers::sPost()
{
  XSqlQuery postPost;
  postPost.prepare("SELECT count(*) AS unposted FROM vohead WHERE (NOT vohead_posted)");
  postPost.exec();

  QList<GuiErrorCheck> errors;
  errors<< GuiErrorCheck(postPost.first() && postPost.value("unposted").toInt()==0, _post,
                         tr("There are no Vouchers to post."))
  ;
  if (GuiErrorCheck::reportErrors(this, tr("No Vouchers to Post"), errors))
    return;

  int journalNumber = 0;

  postPost.exec("SELECT fetchJournalNumber('AP-VO') AS journal;");
  if (postPost.first())
    journalNumber = postPost.value("journal").toInt();
  else if (ErrorReporter::error(QtCriticalMsg, this, tr("Error fetching journal number"),
                                postPost, __FILE__, __LINE__))
    return;

  int succeeded = 0;
  QList<QString> failedItems;
  QList<QString> postErrors;

  XSqlQuery vouchers("SELECT vohead_id, vohead_number "
                     "  FROM vohead "
                     " WHERE NOT vohead_posted;");
  while (vouchers.next())
  {
    XSqlQuery rollback;
    rollback.prepare("ROLLBACK;");

    XSqlQuery("BEGIN;");

    postPost.prepare("SELECT postVoucher(:vohead_id, :journal, false) AS result;");
    postPost.bindValue(":vohead_id", vouchers.value("vohead_id").toInt());
    postPost.bindValue(":journal", journalNumber);
    postPost.exec();
    if (postPost.first())
    {
      int result = postPost.value("result").toInt();
      if (result < 0)
      {
        rollback.exec();
        failedItems.append(vouchers.value("vohead_number").toString());
        postErrors.append(storedProcErrorLookup("postVoucher", result));
        continue;
      }

      if (!_taxIntegration->commit("VCH", vouchers.value("vohead_id").toInt()))
      {
        rollback.exec();
        failedItems.append(vouchers.value("vohead_number").toString());
        postErrors.append(_taxIntegration->error());
        continue;
      }
    }
    else if (postPost.lastError().type() != QSqlError::NoError)
    {
      rollback.exec();
      failedItems.append(vouchers.value("vohead_number").toString());
      postErrors.append(postPost.lastError().text());
      continue;
    }

    succeeded++;
    XSqlQuery("COMMIT;");
  }

  if (postErrors.size() > 0)
  {
    QMessageBox dlg(QMessageBox::Critical, "Errors Posting Voucher", "", QMessageBox::Ok, this);
    dlg.setText(tr("%1 Vouchers succeeded.\n%2 Vouchers failed.").arg(succeeded).arg(failedItems.size()));

    QString details;
    for (int i=0; i<failedItems.size(); i++)
      details += tr("Voucher number %1 failed with:\n%2\n").arg(failedItems[i]).arg(postErrors[i]);
    dlg.setDetailedText(details);

    dlg.exec();
  }

  omfgThis->sVouchersUpdated();
 
  if (_printJournal->isChecked())
  {
    ParameterList params;
    params.append("source", "A/P");
    params.append("sourceLit", tr("A/P"));
    params.append("startJrnlnum", journalNumber);
    params.append("endJrnlnum", journalNumber);

    if (_metrics->boolean("UseJournals"))
    {
      params.append("title",tr("Journal Series"));
      params.append("table", "sltrans");
    }
    else
    {
      params.append("title",tr("General Ledger Series"));
      params.append("gltrans", true);
      params.append("table", "gltrans");
    }

    orReport report("GLSeries", params);
    if (report.isValid())
      report.print();
    else
      report.reportError(this);
  }

  accept();
}
