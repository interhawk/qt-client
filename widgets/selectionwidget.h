/*
* This file is part of the xTuple ERP: PostBooks Edition, a free and
* open source Enterprise Resource Planning software suite,
* Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
* It is licensed to you under the Common Public Attribution License
* version 1.0, the full text of which (including xTuple-specific Exhibits)
* is available at www.xtuple.com/CPAL.  By using this software, you agree
* to be bound by its terms.
*/

#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

#include "widgets.h"
#include <parameter.h>
#include <errorReporter.h>
#include "xsqlquery.h"
#include "ui_selectionwidget.h"


class SelectionWidget : public QWidget, public Ui::SelectionWidget
{
  Q_OBJECT

public:


  SelectionWidget(QWidget* parent = 0, Qt::WindowFlags fl = 0);
  ~SelectionWidget();

  Q_INVOKABLE inline XTreeWidget  *getAvail() const { return _avail; }
  Q_INVOKABLE inline XTreeWidget  *getSel() const { return _sel; }
  Q_INVOKABLE inline QList<XTreeWidgetItem*> getAdded() const { return _added; }
  Q_INVOKABLE inline QList<XTreeWidgetItem*> getRemoved() const { return _removed; }
  Q_INVOKABLE inline QString getAddTableName() const { return _addTableName; }
  Q_INVOKABLE inline QString getRemoveTableName() const { return _removeTableName; }

  Q_INVOKABLE void hideCancel();
  Q_INVOKABLE void showCancel();
  Q_INVOKABLE void hideSelect();
  Q_INVOKABLE void showSelect();
  Q_INVOKABLE void setAddTableName(QString name) { _addTableName = name; }
  Q_INVOKABLE void setRemoveTableName(QString name) { _removeTableName = name; }

  


  public slots:
  virtual void sAdd();
  virtual void sRemove();
  virtual void sAddAll();
  virtual void sRemoveAll();
  virtual void sSelect();
  virtual void sCancel();

signals:
  void addClicked();
  void removeClicked();
  void addAllClicked();
  void removeAllClicked();
  void selectClicked();
  void cancelClicked();
  void itemAdded(int id);
  void itemRemoved(int id);

protected:
  QList<XTreeWidgetItem*> _added;
  QList<XTreeWidgetItem*> _removed;
  QString _addTableName;
  QString _removeTableName;
  QString _removeByIdColName;
  ParameterList _addConstraints;
  ParameterList _removeConstraints;
  bool _removeByAltId;
  void move(XTreeWidgetItem *pXtitem, bool pAdd);
  void buildAddQuery();
};

void setupSelectionWidget(QScriptEngine *engine);

#endif // SELECTIONWIDGET_H
