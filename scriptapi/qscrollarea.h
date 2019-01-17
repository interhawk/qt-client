/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QSCROLLAREAPROTO_H__
#define __QSCROLLAREAPROTO_H__

#include <QScrollArea>
#include <QObject>
#include <QtScript>

Q_DECLARE_METATYPE(QScrollArea*)

void setupQScrollAreaProto(QScriptEngine *engine);
QScriptValue constructQScrollArea(QScriptContext *context, QScriptEngine *engine);

class QScrollAreaProto : public QObject, public QScriptable
{
  Q_OBJECT

  public:
    QScrollAreaProto(QObject *parent);
    Q_INVOKABLE void     ensureVisible(int x, int y, int xmargin = 50, int ymargin = 50);
    Q_INVOKABLE void     ensureWidgetVisible(QWidget *childWidget, int xmargin = 50, int ymargin = 50);
    Q_INVOKABLE void     setWidget(QWidget *widget);
    Q_INVOKABLE void     setWidgetResizable(bool resizable);
    Q_INVOKABLE QWidget* takeWidget();
    Q_INVOKABLE QWidget* widget() const;
    Q_INVOKABLE bool     focusNextPrevChild(bool next);
    Q_INVOKABLE QSize    sizeHint() const;
};

#endif
