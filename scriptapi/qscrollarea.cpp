/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "qscrollarea.h"

#include <QWidget>
#include <QSize>

void setupQScrollAreaProto(QScriptEngine *engine)
{

  QScriptValue constructor = engine->newFunction(constructQScrollArea);
  QScriptValue metaObject = engine->newQMetaObject(&QScrollArea::staticMetaObject, constructor);
  engine->globalObject().setProperty("QScrollArea", metaObject);

  QScriptValue proto = engine->newQObject(new QScrollAreaProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<QScrollArea*>(), proto);
}

QScriptValue constructQScrollArea(QScriptContext *context, QScriptEngine *engine)
{
  QScrollArea *obj = 0;
  if (context->argumentCount() == 1)
    obj = new QScrollArea(qobject_cast<QWidget*>(context->argument(0).toQObject()));
  else
    obj = new QScrollArea();
  return engine->newQObject(obj);
}

QScrollAreaProto::QScrollAreaProto(QObject *parent) : QObject(parent)
{
}

void QScrollAreaProto::ensureVisible(int x, int y, int xmargin, int ymargin)
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    item->ensureVisible(x, y, xmargin, ymargin);
}

void QScrollAreaProto::ensureWidgetVisible(QWidget *childWidget, int xmargin, int ymargin)
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    item->ensureWidgetVisible(childWidget, xmargin, ymargin);
}

void QScrollAreaProto::setWidget(QWidget *widget)
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    item->setWidget(widget);
}

void QScrollAreaProto::setWidgetResizable(bool resizable)
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    item->setWidgetResizable(resizable);
}

QWidget* QScrollAreaProto::takeWidget()
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    return item->takeWidget();
  return new QWidget();
}

QWidget* QScrollAreaProto::widget() const
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    return item->widget();
  return new QWidget();
}

bool QScrollAreaProto::focusNextPrevChild(bool next)
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    return item->focusNextPrevChild(next);
  return false;
}

QSize QScrollAreaProto::sizeHint() const
{
  QScrollArea *item = qscriptvalue_cast<QScrollArea*>(thisObject());
  if (item)
    return item->sizeHint();
  return QSize();
}
