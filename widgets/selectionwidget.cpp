/*
* This file is part of the xTuple ERP: PostBooks Edition, a free and
* open source Enterprise Resource Planning software suite,
* Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
* It is licensed to you under the Common Public Attribution License
* version 1.0, the full text of which (including xTuple-specific Exhibits)
* is available at www.xtuple.com/CPAL.  By using this software, you agree
* to be bound by its terms.
*/

#include "selectionwidget.h"


#define DEBUG false

SelectionWidget::SelectionWidget(QWidget *parent, Qt::WindowFlags fl)
  : QWidget(parent, fl)
{
  setupUi(this);

  _modifyTableName = "";
  _removeByIdTableColName = "";
  _removeByAltId = false;
  connect(_add, SIGNAL(clicked()), this, SLOT(sAdd()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(sRemove()));
  connect(_addAll, SIGNAL(clicked()), this, SLOT(sAddAll()));
  connect(_removeAll, SIGNAL(clicked()), this, SLOT(sRemoveAll()));
  connect(_select, SIGNAL(clicked()), this, SLOT(sSelect()));
  connect(_cancel, SIGNAL(clicked()), this, SLOT(sCancel()));
  //connect both signals so population order doesn't matter
  connect(_avail, SIGNAL(populated()), this, SLOT(sFilterDuplicates()));
  connect(_sel, SIGNAL(populated()), this, SLOT(sFilterDuplicates()));
}

SelectionWidget::~SelectionWidget()
{
}

void SelectionWidget::sSelect() { emit selectClicked(); }
void SelectionWidget::sCancel() { emit cancelClicked(); }
void SelectionWidget::hideCancel() { _cancel->hide(); }
void SelectionWidget::showCancel() { _cancel->show(); }
void SelectionWidget::hideSelect() { _select->hide(); }
void SelectionWidget::showSelect() { _select->show(); }

void SelectionWidget::sAdd()
{
  emit addClickedBefore();
  foreach(XTreeWidgetItem *xtitem, _avail->selectedItems())
  {
    move(xtitem, true);
    emit itemAdded(xtitem->id());
  }
  emit addClickedAfter();
}

void SelectionWidget::sRemove()
{
  emit removeClickedBefore();
  foreach(XTreeWidgetItem *xtitem, _sel->selectedItems())
  {
    move(xtitem, false);
    emit itemRemoved(xtitem->id());
  }
  emit removeClickedAfter();
}

void SelectionWidget::sAddAll()
{
  emit addAllClickedBefore();
  for (int i = _avail->topLevelItemCount() -1; i >= 0; i--) //loop backwards since we're removing items.
  {
    XTreeWidgetItem *xtitem = dynamic_cast<XTreeWidgetItem*>(_avail->takeTopLevelItem(i));
    move(xtitem, true);
    emit itemAdded(xtitem->id());
  }
  emit addAllClickedAfter();
}

void SelectionWidget::sRemoveAll()
{
  emit removeAllClickedBefore();
  for (int i = _sel->topLevelItemCount() -1; i >= 0 ; i--) //loop backwards since we're removing items.
  {
    XTreeWidgetItem *xtitem = dynamic_cast<XTreeWidgetItem*>(_sel->takeTopLevelItem(i));
    move(xtitem, false);
    emit itemRemoved(xtitem->id());
  }
  emit removeAllClickedAfter();
}


void SelectionWidget::move(XTreeWidgetItem *pXtitem, bool pAdd)
{
  XTreeWidget *sourceTree;
  XTreeWidget *destTree;
  QList<XTreeWidgetItem*> *checkList;
  QList<XTreeWidgetItem*> *appendList;

  if (pAdd)
  {
    sourceTree = _avail;
    destTree = _sel;
    checkList = &_removed;
    appendList = &_added;
  }
  else
  {
    sourceTree = _sel;
    destTree = _avail;
    checkList = &_added;
    appendList = &_removed;
  }
  
  sourceTree->takeTopLevelItem(sourceTree->indexOfTopLevelItem(pXtitem));
  if (!checkList->removeOne(pXtitem))
  {
    appendList->append(pXtitem);
  }
  destTree->addTopLevelItem(pXtitem);
}

bool SelectionWidget::isSameItem(XTreeWidgetItem *pXtitem1, XTreeWidgetItem *pXtitem2) const
{
  int index = 0;
  while (index < _equalityColumns.count())
  {
    if (pXtitem1->rawValue(_equalityColumns.name(index)) !=
      pXtitem2->rawValue(_equalityColumns.value(index).toString()))
    {
      return false;
    }
    index++;
  }
  return true;
}

void SelectionWidget::sFilterDuplicates()
{
  //maybe a hash might be better here...
  for (int availIndex = _avail->topLevelItemCount() - 1; availIndex >= 0; availIndex--)
  {
    bool bFound = false;
    int selIndex = 0;
    while (selIndex < _sel->topLevelItemCount() && !bFound)
    {
      if (isSameItem(_avail->topLevelItem(availIndex), _sel->topLevelItem(selIndex)))
      {
        _avail->takeTopLevelItem(availIndex);
        bFound = true;
      }
      selIndex++;
    }
  }
}

int SelectionWidget::buildAddQuery()
{
  XSqlQuery qry;
  XSqlQuery rollback;
  rollback.prepare("ROLLBACK;");
  qry.exec("BEGIN;");

  foreach(XTreeWidgetItem *xtitem, _added)
  {
    QString qryString = "INSERT INTO ";
    qryString.append(_modifyTableName + " (");
    for (int i = 0; i < _addConstraints.count(); i++)
    {
      qryString.append(_addConstraints.name(i) + ", ");
    }
    qryString.chop(2);//remove the ", " from the last iteration.
    qryString.append(") VALUES (");
    for (int i = 0; i < _addConstraints.count(); i++)
    {
      qryString.append(QString(":parameter%1").arg(i) + ", ");
    }
    qryString.chop(2);//remove the ", " from the last iteration.
    qryString.append(");");
    qry.prepare(qryString);
    QVariant bindVal;
    for (int i = 0; i < _addConstraints.count(); i++)
    {
      bindVal = xtitem->rawValue(_addConstraints.value(i).toString());
      if (bindVal.isNull())
      {
        bindVal = _addConstraints.value(i);
      }
      qry.bindValue(QString(":parameter%1").arg(i), bindVal);
    }
    if (DEBUG)
    {
      qDebug("Add query string: %1", qryString);
      QMapIterator<QString, QVariant> bindings(qry.boundValues());
      while (bindings.hasNext())
      {
        bindings.next();
        qDebug("%1:\t%2", bindings.key(), bindings.value().toString());
      }
    }
    qry.exec();
    if (qry.lastError().type() != QSqlError::NoError)
    {
      rollback.exec();
      ErrorReporter::error(QtCriticalMsg, this, tr("Error Adding Selected items."),
                           qry, __FILE__, __LINE__);
      return(-1);
    }
    
  }
  qry.exec("COMMIT;");
  return(0);
}

int SelectionWidget::buildRemoveQuery()
{
  XSqlQuery qry;
  QString qryString = "DELETE FROM ";
  qryString.append(_modifyTableName + " WHERE " + _removeByIdTableColName + " IN (");
  for (int i = 0; i < _removed.count(); i++)
  {
    qryString.append(QString(":parameter%1").arg(i) + ", ");
  }
  qryString.chop(2);//remove the ", " from the last iteration.
  qryString.append(");");
  qry.prepare(qryString);
  int counter = 0;
  foreach(XTreeWidgetItem *xtitem, _removed)
  {
    int remove = _removeByAltId ? xtitem->altId() : xtitem->id();
    qry.bindValue(QString(":parameter%1").arg(counter), remove);
    counter++;
  }
  if (DEBUG)
  {
    qDebug("Remove query string: %1", qryString);
    QMapIterator<QString, QVariant> bindings(qry.boundValues());
    while (bindings.hasNext())
    {
      bindings.next();
      qDebug("%1:\t%2", bindings.key(), bindings.value().toString());
    }
  }
  qry.exec();
  if (ErrorReporter::error(QtCriticalMsg, this, tr("Error Adding Selected items."),
                           qry, __FILE__, __LINE__))
  {
    return(-1);
  }
  return(0);
}


void SelectionWidget::sCommitChanges()
{
  if ((!_modifyTableName.isEmpty()) && (!_addConstraints.isEmpty()) && (!_added.isEmpty()))
  {
    if (buildAddQuery() == 0)
    {
      _added.clear();
    }
  }
  if ((!_modifyTableName.isEmpty()) && (!_removeByIdTableColName.isEmpty()) && (!_removed.isEmpty()))
  {
    if (buildRemoveQuery() == 0)
    {
      _removed.clear();
    }
  }
}

// script exposure ////////////////////////////////////////////////////////////
QScriptValue SelectionWidgetToScriptValue(QScriptEngine *engine, SelectionWidget *const &obj)
{
  return engine->newQObject(obj);
}
void SelectionWidgetFromScriptValue(const QScriptValue &obj, SelectionWidget* &item)
{
  item = qobject_cast<SelectionWidget*>(obj.toQObject());
}
void setupSelectionWidget(QScriptEngine *engine)
{
  qScriptRegisterMetaType(engine, SelectionWidgetToScriptValue, SelectionWidgetFromScriptValue);
  if (!engine->globalObject().property("SelectionWidget").isObject())
  {
      QScriptValue ctor = engine->newObject();
      QScriptValue meta = engine->newQMetaObject(&SelectionWidget::staticMetaObject, ctor);

      engine->globalObject().setProperty("SelectionWidget", meta, 
                                          QScriptValue::ReadOnly | QScriptValue::Undeletable);
  }
}
