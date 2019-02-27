/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
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
  _mode = cNew;

  if (_x_taxIntegration)
    connect(_x_taxIntegration, SIGNAL(taxCalculated(double, QString)), this, SLOT(sUpdate(double, QString)));

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

  connect(menu, SIGNAL(aboutToShow()), this, SLOT(sUpdateMenu()));
}

QString TaxDisplay::type()
{
  return _type;
}

int TaxDisplay::orderId()
{
  return _orderId;
}

int TaxDisplay::mode()
{
  return _mode;
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

void TaxDisplay::setMode(int mode)
{
  bool refresh = (_mode != mode);
  _mode = mode;

  if (refresh)
    sRefresh();
}

void TaxDisplay::sRecalculate()
{
  emit save(true);
  if (_x_taxIntegration)
    _x_taxIntegration->calculateTax(_type, _orderId);
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

  if (_x_taxIntegration)
    _x_taxIntegration->wait();

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
  {
    clear();
    QMessageBox::critical(0, tr("Avalara Error"), tr("Error Calculating Tax:\n%1").arg(error));
  }

  _menuLabel->setPixmap(QPixmap(":/widgets/images/gear.png"));
}

void TaxDisplay::sRefresh()
{
  if (_orderId < 0 || _mode == cNew)
    return;

  XSqlQuery tax;
  tax.prepare("SELECT getOrderTax(:type, :id) AS tax;");
  tax.bindValue(":type", _type);
  tax.bindValue(":id", _orderId);
  tax.exec();
  if (tax.first())
    setLocalValue(tax.value("tax").toDouble());
  else
    clear();
  if (ErrorReporter::error(QtCriticalMsg, 0, tr("Error fetching tax"),
                           tax, __FILE__, __LINE__))
    return;

  if (_x_metrics && _x_metrics->value("TaxService") == "A")
  {
    tax.prepare("SELECT taxhead_valid AS valid "
                "  FROM taxhead "
                " WHERE taxhead_doc_type = :type "
                "   AND taxhead_doc_id = :id;");
    tax.bindValue(":type", _type);
    tax.bindValue(":id", _orderId);
    tax.exec();
    if (tax.first() && !tax.value("valid").toBool())
      _menuLabel->setPixmap(QPixmap(":/widgets/images/warning.png"));
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error fetching tax"),
                         tax, __FILE__, __LINE__);
  }
}

void TaxDisplay::save()
{
  if (_x_taxIntegration)
    _x_taxIntegration->calculateTax(_type, _orderId, true);
}

void TaxDisplay::invalidate()
{
  if (_x_metrics && _x_metrics->value("TaxService") == "A")
  {
    if (isEmpty())
      return;

    _menuLabel->setPixmap(QPixmap(":/widgets/images/warning.png"));

    XSqlQuery tax;
    tax.prepare("UPDATE taxhead "
                "   SET taxhead_valid = FALSE "
                " WHERE taxhead_doc_type = :type "
                "   AND taxhead_doc_id = :id;");
    tax.bindValue(":type", _type);
    tax.bindValue(":id", _orderId);
    tax.exec();
    ErrorReporter::error(QtCriticalMsg, 0, tr("Error invalidating tax"),
                         tax, __FILE__, __LINE__);
  }
  else
    sRecalculate();
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

void TaxDisplay::sUpdateMenu()
{
  if (isEmpty())
  {
    _recalculateAct->setText(tr("Calculate Tax"));
    _openAct->setEnabled(false);
  }
  else
  {
    _recalculateAct->setText(tr("Recalculate Tax"));
    _openAct->setEnabled(true);
  }

  _recalculateAct->setVisible(_x_metrics ? _x_metrics->value("TaxService") == "A" : false);
  _recalculateAct->setEnabled(_mode != cView);
}
