/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "qtablewidgetproto.h"

#include <QWidget>

void setupQTableWidgetProto(QScriptEngine *engine)
{

  QScriptValue constructor = engine->newFunction(constructQTableWidget);
  QScriptValue metaObject = engine->newQMetaObject(&QTableWidget::staticMetaObject, constructor);
  engine->globalObject().setProperty("QTableWidget", metaObject);

  QScriptValue proto = engine->newQObject(new QTableWidgetProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<QTableWidget*>(), proto);
}

QScriptValue constructQTableWidget(QScriptContext *context, QScriptEngine *engine)
{
  QTableWidget *obj = 0;
  if (context->argumentCount() == 1)
    obj = new QTableWidget(qobject_cast<QWidget*>(context->argument(0).toQObject()));
  else
    obj = new QTableWidget();
  return engine->newQObject(obj);
}

QTableWidgetProto::QTableWidgetProto(QObject *parent) : QObject(parent)
{
}

QWidget* QTableWidgetProto::cellWidget(int row, int column) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->cellWidget(row, column);
  return 0;
}

void QTableWidgetProto::closePersistentEditor(QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->closePersistentEditor(item);
}

int QTableWidgetProto::column(const QTableWidgetItem *item) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->column(item);
  return 0;
}

int QTableWidgetProto::currentColumn() const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->currentColumn();
  return 0;
}

QTableWidgetItem* QTableWidgetProto::currentItem() const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->currentItem();
  return 0;
}

int QTableWidgetProto::currentRow() const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
   return  item_proto->currentRow();
  return 0;
}

void QTableWidgetProto::editItem(QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->editItem(item);
}

QList<QTableWidgetItem *> QTableWidgetProto::findItems(const QString &text, Qt::MatchFlags flags) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->findItems(text, flags);
  return QList<QTableWidgetItem *>();
  
}

QTableWidgetItem* QTableWidgetProto::horizontalHeaderItem(int column) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->horizontalHeaderItem(column);
  return 0;
}

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
bool QTableWidgetProto::isPersistentEditorOpen(QTableWidgetItem *item) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->isPersistentEditorOpen(item);
  return false;
}
#endif

QTableWidgetItem* QTableWidgetProto::item(int row, int column) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->item(row, column);
  return 0;
}

QTableWidgetItem* QTableWidgetProto::itemAt(const QPoint &point) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->itemAt(point);
  return 0;
}

QTableWidgetItem* QTableWidgetProto::itemAt(int ax, int ay) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->itemAt(ax, ay);
  return 0;
}

const QTableWidgetItem * QTableWidgetProto::itemPrototype() const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->itemPrototype();
  return 0;
}

void QTableWidgetProto::openPersistentEditor(QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->openPersistentEditor(item);
}

void QTableWidgetProto::removeCellWidget(int row, int column)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->removeCellWidget(row, column);
}

int QTableWidgetProto::row(const QTableWidgetItem *item) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->row(item);
  return 0;
}

QList<QTableWidgetItem *> QTableWidgetProto::selectedItems() const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->selectedItems();
  return QList<QTableWidgetItem *>();
}

QList<QTableWidgetSelectionRange> QTableWidgetProto::selectedRanges() const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->selectedRanges();
  return QList<QTableWidgetSelectionRange>();
}

void QTableWidgetProto::setCellWidget(int row, int column, QWidget *widget)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setCellWidget(row, column, widget);
}

void QTableWidgetProto::setColumnCount(int columns)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setColumnCount(columns);
}

void QTableWidgetProto::setCurrentCell(int row, int column)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setCurrentCell(row, column);
}

void QTableWidgetProto::setCurrentCell(int row, int column, QItemSelectionModel::SelectionFlags command)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setCurrentCell(row, column, command);
}

void QTableWidgetProto::setCurrentItem(QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setCurrentItem(item);
}

void QTableWidgetProto::setCurrentItem(QTableWidgetItem *item, QItemSelectionModel::SelectionFlags command)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setCurrentItem(item, command);
}

void QTableWidgetProto::setHorizontalHeaderItem(int column, QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setHorizontalHeaderItem(column, item);
}

void QTableWidgetProto::setHorizontalHeaderLabels(const QStringList &labels)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setHorizontalHeaderLabels(labels);
}

void QTableWidgetProto::setItem(int row, int column, QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setItem(row, column, item);
}

void QTableWidgetProto::setItemPrototype(const QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setItemPrototype(item);
}

void QTableWidgetProto::setRangeSelected(const QTableWidgetSelectionRange &range, bool select)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setRangeSelected(range, select);
}

void QTableWidgetProto::setRowCount(int rows)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setRowCount(rows);
}

void QTableWidgetProto::setVerticalHeaderItem(int row, QTableWidgetItem *item)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setVerticalHeaderItem(row, item);
}

void QTableWidgetProto::setVerticalHeaderLabels(const QStringList &labels)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->setVerticalHeaderLabels(labels);
}

void QTableWidgetProto::sortItems(int column, Qt::SortOrder order)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    item_proto->sortItems(column, order);
}

QTableWidgetItem* QTableWidgetProto::takeHorizontalHeaderItem(int column)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->takeHorizontalHeaderItem(column);
  return 0;
}

QTableWidgetItem* QTableWidgetProto::takeItem(int row, int column)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->takeItem(row, column);
  return 0;
}

QTableWidgetItem* QTableWidgetProto::takeVerticalHeaderItem(int row)
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
   return item_proto->takeVerticalHeaderItem(row);
 return 0;
}

QTableWidgetItem* QTableWidgetProto::verticalHeaderItem(int row) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->verticalHeaderItem(row);
  return 0;
}

int QTableWidgetProto::visualColumn(int logicalColumn) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->visualColumn(logicalColumn);
  return 0;
}

QRect QTableWidgetProto::visualItemRect(const QTableWidgetItem *item) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->visualItemRect(item);
  return QRect(); 
}

int QTableWidgetProto::visualRow(int logicalRow) const
{
  QTableWidget *item_proto = qscriptvalue_cast<QTableWidget*>(thisObject());
  if (item_proto)
    return item_proto->visualRow(logicalRow);
  return 0;
}
