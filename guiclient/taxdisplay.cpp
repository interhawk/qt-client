/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "taxdisplay.h"

#include <QMenu>
#include <QMessageBox>

#include "errorReporter.h"
#include "taxBreakdown.h"

TaxDisplay::TaxDisplay(QWidget* parent, const char* name)
  : CurrDisplay(parent, name)
{
  _type = "";
  _orderId = -1;

  _tax = TaxIntegration::getTaxIntegration();

  connect(_tax, SIGNAL(taxCalculated(double, QString)), this, SLOT(sUpdate(double, QString)));

  _recalculateAct = new QAction(tr("Recalculate Tax"), this);
  _recalculateAct->setObjectName("_recalculateAct");
  connect(_recalculateAct, SIGNAL(triggered()), this, SLOT(sRecalculate()));

  _openAct = new QAction(tr("Tax Breakdown..."), this);
  _openAct->setObjectName("_openAct");
  connect(_openAct, SIGNAL(triggered()), this, SLOT(sOpen()));

  _menuLabel = new QLabel(this);
  _menuLabel->setObjectName("_menuLabel");
  _menuLabel->setPixmap(QPixmap(":/widgets/images/gear.png"));
  _menuLabel->installEventFilter(this);

  int height = _valueLocalWidget->minimumSizeHint().height();
  QString sheet = QLatin1String("QLineEdit{ padding-left: ");
  sheet += QString::number(_menuLabel->pixmap()->width() + 6);
  sheet += QLatin1String(";}");
  setStyleSheet(sheet);
  // Little hack. Somehow style sheet makes widget short. Put back height.
  _valueLocalWidget->setMinimumHeight(height);

  QMenu* menu = new QMenu;
  menu->setObjectName("menu");
  menu->addAction(_recalculateAct);
  menu->addAction(_openAct);

  _menu = menu;
}

QString TaxDisplay::type()
{
  return _type;
}

int TaxDisplay::orderId()
{
  return _orderId;
}

void TaxDisplay::setType(QString type)
{
  bool refresh = (_type != type);
  _type = type;

  if (refresh)
    sRefresh();
}

void TaxDisplay::setOrderId(int id)
{
  bool refresh = (_orderId != id);
  _orderId = id;

  if (refresh)
    sRefresh();
}

void TaxDisplay::sRecalculate()
{
  _tax->calculateTax(_type, _orderId);
}

void TaxDisplay::sOpen()
{
  QString mode;

  if (_type == "COB" || _type == "INV")
    mode = "edit";
  else
    mode = "view";

  ParameterList params;
  params.append("order_type", _type);
  params.append("order_id", _orderId);
  params.append("mode", "mode");

  _tax->wait();

  taxBreakdown newdlg(0, "", true);
  newdlg.set(params);
  newdlg.exec();

  if (mode == "edit")
    sRefresh();
}

void TaxDisplay::sUpdate(double tax, QString error)
{
  if (error.isEmpty())
    setLocalValue(tax);
  else
    QMessageBox::critical(0, tr("Avalara Error"), tr("Error Calculating Tax:\n%1").arg(error));
}

void TaxDisplay::sRefresh()
{
  if (_orderId < 0)
    return;

  XSqlQuery tax;
  tax.prepare("SELECT getOrderTax(:type, :id) AS tax;");
  tax.bindValue(":type", _type);
  tax.bindValue(":id", _orderId);
  tax.exec();
  if (tax.first())
    setLocalValue(tax.value("tax").toDouble());
  ErrorReporter::error(QtCriticalMsg, 0, tr("Error fetching tax"),
                       tax, __FILE__, __LINE__);
}

void TaxDisplay::save()
{
  _tax->calculateTax(_type, _orderId, true);
}

void TaxDisplay::post()
{
  _tax->commit(_type, _orderId);
}

bool TaxDisplay::eventFilter(QObject *obj, QEvent *event)
{
    if (!_menu || obj != _menuLabel)
        return QObject::eventFilter(obj, event);

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        const QMouseEvent *me = static_cast<QMouseEvent *>(event);
        _menu->exec(me->globalPos());
        return true;
    }
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

void TaxDisplay::resizeEvent(QResizeEvent *)
{
  positionMenuLabel();
}

void TaxDisplay::positionMenuLabel()
{
  _menuLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  _menuLabel->setStyleSheet("QLabel { margin-left:6}");

  _menuLabel->setGeometry(0, 0, _menuLabel->pixmap()->width() + 6, height());
}
