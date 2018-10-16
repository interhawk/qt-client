/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QTABLEWIDGETPROTO_H__
#define __QTABLEWIDGETPROTO_H__

#include <QTableWidget>
#include <QObject>
#include <QtScript>

Q_DECLARE_METATYPE(QTableWidget*)

void setupQTableWidgetProto(QScriptEngine *engine);
QScriptValue constructQTableWidget(QScriptContext *context, QScriptEngine *engine);

class QTableWidgetProto : public QObject, public QScriptable
{
  Q_OBJECT

  public:
    QTableWidgetProto(QObject *parent);
    Q_INVOKABLE QWidget *                         cellWidget(int row, int column) const;
    Q_INVOKABLE void                              closePersistentEditor(QTableWidgetItem *item);
    Q_INVOKABLE int                               column(const QTableWidgetItem *item) const;
    Q_INVOKABLE int                               currentColumn() const;
    Q_INVOKABLE QTableWidgetItem *                currentItem() const;
    Q_INVOKABLE int                               currentRow() const;
    Q_INVOKABLE void                              editItem(QTableWidgetItem *item);
    Q_INVOKABLE QList<QTableWidgetItem *>         findItems(const QString &text, Qt::MatchFlags flags) const;
    Q_INVOKABLE QTableWidgetItem *                horizontalHeaderItem(int column) const;
    #if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    Q_INVOKABLE bool                              isPersistentEditorOpen(QTableWidgetItem *item) const;
    #endif
    Q_INVOKABLE QTableWidgetItem *                item(int row, int column) const;
    Q_INVOKABLE QTableWidgetItem *                itemAt(const QPoint &point) const;
    Q_INVOKABLE QTableWidgetItem *                itemAt(int ax, int ay) const;
    Q_INVOKABLE const QTableWidgetItem *          itemPrototype() const;
    Q_INVOKABLE void                              openPersistentEditor(QTableWidgetItem *item);
    Q_INVOKABLE void                              removeCellWidget(int row, int column);
    Q_INVOKABLE int                               row(const QTableWidgetItem *item) const;
    Q_INVOKABLE QList<QTableWidgetItem *>         selectedItems() const;
    Q_INVOKABLE QList<QTableWidgetSelectionRange> selectedRanges() const;
    Q_INVOKABLE void                              setCellWidget(int row, int column, QWidget *widget);
    Q_INVOKABLE void                              setColumnCount(int columns);
    Q_INVOKABLE void                              setCurrentCell(int row, int column);
    Q_INVOKABLE void                              setCurrentCell(int row, int column, QItemSelectionModel::SelectionFlags command);
    Q_INVOKABLE void                              setCurrentItem(QTableWidgetItem *item);
    Q_INVOKABLE void                              setCurrentItem(QTableWidgetItem *item, QItemSelectionModel::SelectionFlags command);
    Q_INVOKABLE void                              setHorizontalHeaderItem(int column, QTableWidgetItem *item);
    Q_INVOKABLE void                              setHorizontalHeaderLabels(const QStringList &labels);
    Q_INVOKABLE void                              setItem(int row, int column, QTableWidgetItem *item);
    Q_INVOKABLE void                              setItemPrototype(const QTableWidgetItem *item);
    Q_INVOKABLE void                              setRangeSelected(const QTableWidgetSelectionRange &range, bool select);
    Q_INVOKABLE void                              setRowCount(int rows);
    Q_INVOKABLE void                              setVerticalHeaderItem(int row, QTableWidgetItem *item);
    Q_INVOKABLE void                              setVerticalHeaderLabels(const QStringList &labels);
    Q_INVOKABLE void                              sortItems(int column, Qt::SortOrder order = Qt::AscendingOrder);
    Q_INVOKABLE QTableWidgetItem *                takeHorizontalHeaderItem(int column);
    Q_INVOKABLE QTableWidgetItem *                takeItem(int row, int column);
    Q_INVOKABLE QTableWidgetItem *                takeVerticalHeaderItem(int row);
    Q_INVOKABLE QTableWidgetItem *                verticalHeaderItem(int row) const;
    Q_INVOKABLE int                               visualColumn(int logicalColumn) const;
    Q_INVOKABLE QRect                             visualItemRect(const QTableWidgetItem *item) const;
    Q_INVOKABLE int                               visualRow(int logicalRow) const;
};

#endif