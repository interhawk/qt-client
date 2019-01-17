/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */


#include "qtablewidgetitemproto.h"
#include "scriptapi_internal.h"

#include <QWidget>
#include <qiconproto.h>

QScriptValue ItemTypeToScriptValue(QScriptEngine *engine, const enum QTableWidgetItem::ItemType &p)
{
  return QScriptValue(engine, (int)p);
}
void ItemTypeFromScriptValue(const QScriptValue &obj, enum QTableWidgetItem::ItemType &p)
{
  p = (enum QTableWidgetItem::ItemType)obj.toInt32();
}

void setupQTableWidgetItemProto(QScriptEngine *engine)
{
  QScriptValue proto = engine->newQObject(new QTableWidgetItemProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<QTableWidgetItem*>(), proto);
  engine->setDefaultPrototype(qMetaTypeId<QTableWidgetItem>(), proto);
  QScriptValue constructor = engine->newFunction(constructQTableWidgetItem, proto);
  engine->globalObject().setProperty("QTableWidgetItem",  constructor);
  qScriptRegisterMetaType(engine, ItemTypeToScriptValue, ItemTypeFromScriptValue);
  constructor.setProperty("Type",   QScriptValue(engine, QTableWidgetItem::Type),   ENUMPROPFLAGS);
  constructor.setProperty("UserType", QScriptValue(engine, QTableWidgetItem::UserType), ENUMPROPFLAGS);
}

QScriptValue constructQTableWidgetItem(QScriptContext *context, QScriptEngine *engine)
{
  QTableWidgetItem *obj = 0;
  
  if (context->argumentCount() == 0)
    obj = new QTableWidgetItem();
  else if (context->argumentCount() == 1)
  {
    if (context->argument(0).isNumber())
      obj = new QTableWidgetItem(context->argument(0).toInt32());
    else if (context->argument(0).isString())
      obj = new QTableWidgetItem(context->argument(0).toString());
    else if (context->argument(0).isObject() &&
             qscriptvalue_cast<QTableWidgetItem *>(context->argument(0).toObject()))
      obj = new QTableWidgetItem(qscriptvalue_cast<QTableWidgetItem>(context->argument(0).toObject()));
  }
  else if (context->argumentCount() == 2)
  {
    if (context->argument(0).isString() &&
        context->argument(1).isNumber())
      obj = new QTableWidgetItem(context->argument(0).toString(),
                                 context->argument(1).toInt32());
    else if (context->argument(0).isObject() &&
             context->argument(1).isString() &&
             qscriptvalue_cast<QIcon *>(context->argument(0).toObject()))
      obj = new QTableWidgetItem(qscriptvalue_cast<QIcon>(context->argument(0).toObject()),
                                 context->argument(1).toString());
  }
  else if (context->argumentCount() == 3)
  {
    if (context->argument(0).isObject() &&
        context->argument(1).isString() &&
        context->argument(2).isNumber() &&
        qscriptvalue_cast<QIcon *>(context->argument(0).toObject()))
      obj = new QTableWidgetItem(qscriptvalue_cast<QIcon>(context->argument(0).toObject()),
                                 context->argument(1).toString(),
                                 context->argument(2).toInt32());
  }
  else
  {
    context->throwError(QScriptContext::UnknownError,
                        "could not find appropriate QTableWidgetItem constructor");
  }
  
  return engine->toScriptValue(obj);
}

QTableWidgetItemProto::QTableWidgetItemProto(QObject *parent) : QObject(parent)
{
}

QBrush QTableWidgetItemProto::background() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->background();
  return QBrush();
}

Qt::CheckState QTableWidgetItemProto::checkState() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->checkState();
  return (Qt::CheckState)0;
}
 
QTableWidgetItem * QTableWidgetItemProto::clone() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->clone();
  return 0;
}
 
int QTableWidgetItemProto::column() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->column();
  return 0;
}
 
QVariant QTableWidgetItemProto::data(int role) const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->data(role);
  return QVariant();
}
 
Qt::ItemFlags QTableWidgetItemProto::flags() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->flags();
  return (Qt::ItemFlags)0;
}
 
QFont QTableWidgetItemProto::font() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->font();
  return QFont();
}
 
QBrush QTableWidgetItemProto::foreground() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->foreground();
  return QBrush();
}
 
QIcon QTableWidgetItemProto::icon() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->icon();
  return QIcon();
}
 
bool QTableWidgetItemProto::isSelected() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->isSelected();
  return false;
}
 
void QTableWidgetItemProto::read(QDataStream &in)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->read(in);
}
 
int QTableWidgetItemProto::row() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->row();
  return 0;
}
 
void QTableWidgetItemProto::setBackground(const QBrush &brush)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setBackground(brush);
}
 
void QTableWidgetItemProto::setCheckState(Qt::CheckState state)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setCheckState(state);
}
 
void QTableWidgetItemProto::setData(int role, const QVariant &value)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setData(role, value);
}
 
void QTableWidgetItemProto::setFlags(Qt::ItemFlags flags)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setFlags(flags);
}
 
void QTableWidgetItemProto::setFont(const QFont &font)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setFont(font);
}
 
void QTableWidgetItemProto::setForeground(const QBrush &brush)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setForeground(brush);
}
 
void QTableWidgetItemProto::setIcon(const QIcon &icon)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setIcon(icon);
}
 
void QTableWidgetItemProto::setSelected(bool select)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setSelected(select);
}
 
void QTableWidgetItemProto::setSizeHint(const QSize &size)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setSizeHint(size);
}
 
void QTableWidgetItemProto::setStatusTip(const QString &statusTip)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setStatusTip(statusTip);
}
 
void QTableWidgetItemProto::setText(const QString &text)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setText(text);
}
 
void QTableWidgetItemProto::setTextAlignment(int alignment)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setTextAlignment(alignment);
}
 
void QTableWidgetItemProto::setToolTip(const QString &toolTip)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setToolTip(toolTip);
}
 
void QTableWidgetItemProto::setWhatsThis(const QString &whatsThis)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->setWhatsThis(whatsThis);
}
 
QSize QTableWidgetItemProto::sizeHint() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->sizeHint();
  return QSize();
}
 
QString QTableWidgetItemProto::statusTip() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->statusTip();
  return QString();
}
 
QTableWidget * QTableWidgetItemProto::tableWidget() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->tableWidget();
  return 0;
}
 
QString QTableWidgetItemProto::text() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->text();
  return QString();
}
 
int QTableWidgetItemProto::textAlignment() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->textAlignment();
  return 0;
}
 
QString QTableWidgetItemProto::toolTip() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->toolTip();
  return QString();
}
 
int QTableWidgetItemProto::type() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->type();
  return 0;
}
 
QString QTableWidgetItemProto::whatsThis() const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->whatsThis();
  return QString();
}
 
void QTableWidgetItemProto::write(QDataStream &out) const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    item_proto->write(out);
}
 
bool QTableWidgetItemProto::operator<(const QTableWidgetItem &other) const
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->operator<(other);
  return false;
}
 
QTableWidgetItem & QTableWidgetItemProto::operator=(const QTableWidgetItem &other)
{
  QTableWidgetItem *item_proto = qscriptvalue_cast<QTableWidgetItem*>(thisObject());
  if (item_proto)
    return item_proto->operator=(other);
  return  *other.clone();
}
 