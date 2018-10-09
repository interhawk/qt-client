/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QTABLEWIDGETITEMPROTO_H__
#define __QTABLEWIDGETITEMPROTO_H__

#include <QTableWidgetItem>
#include <QObject>
#include <QtScript>

Q_DECLARE_METATYPE(QTableWidgetItem*)
Q_DECLARE_METATYPE(QTableWidgetItem)
Q_DECLARE_METATYPE(enum QTableWidgetItem::ItemType)

void setupQTableWidgetItemProto(QScriptEngine *engine);
QScriptValue constructQTableWidgetItem(QScriptContext *context, QScriptEngine *engine);

class QTableWidgetItemProto : public QObject, public QScriptable
{
  Q_OBJECT

  public:
    QTableWidgetItemProto(QObject *parent);
    Q_INVOKABLE QBrush                     background() const;
    Q_INVOKABLE Qt::CheckState             checkState() const;
    Q_INVOKABLE QTableWidgetItem *         clone() const;
    Q_INVOKABLE int                        column() const;
    Q_INVOKABLE QVariant                   data(int role) const;
    Q_INVOKABLE Qt::ItemFlags              flags() const;
    Q_INVOKABLE QFont                      font() const;
    Q_INVOKABLE QBrush                     foreground() const;
    Q_INVOKABLE QIcon                      icon() const;
    Q_INVOKABLE bool                       isSelected() const;
    Q_INVOKABLE void                       read(QDataStream &in);
    Q_INVOKABLE int                        row() const;
    Q_INVOKABLE void                       setBackground(const QBrush &brush);
    Q_INVOKABLE void                       setCheckState(Qt::CheckState state);
    Q_INVOKABLE void                       setData(int role, const QVariant &value);
    Q_INVOKABLE void                       setFlags(Qt::ItemFlags flags);
    Q_INVOKABLE void                       setFont(const QFont &font);
    Q_INVOKABLE void                       setForeground(const QBrush &brush);
    Q_INVOKABLE void                       setIcon(const QIcon &icon);
    Q_INVOKABLE void                       setSelected(bool select);
    Q_INVOKABLE void                       setSizeHint(const QSize &size);
    Q_INVOKABLE void                       setStatusTip(const QString &statusTip);
    Q_INVOKABLE void                       setText(const QString &text);
    Q_INVOKABLE void                       setTextAlignment(int alignment);
    Q_INVOKABLE void                       setToolTip(const QString &toolTip);
    Q_INVOKABLE void                       setWhatsThis(const QString &whatsThis);
    Q_INVOKABLE QSize                      sizeHint() const;
    Q_INVOKABLE QString                    statusTip() const;
    Q_INVOKABLE QTableWidget *             tableWidget() const;
    Q_INVOKABLE QString                    text() const;
    Q_INVOKABLE int                        textAlignment() const;
    Q_INVOKABLE QString                    toolTip() const;
    Q_INVOKABLE int                        type() const;
    Q_INVOKABLE QString                    whatsThis() const;
    Q_INVOKABLE void                       write(QDataStream &out) const;
    Q_INVOKABLE bool                       operator<(const QTableWidgetItem &other) const;
    Q_INVOKABLE QTableWidgetItem &         operator=(const QTableWidgetItem &other);
};

#endif