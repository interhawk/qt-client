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

  _addTableName = "";
  _removeTableName = "";
  _removeByIdColName = "";
  _removeByAltId = false;
  connect(_add, SIGNAL(clicked()), this, SLOT(sAdd()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(sRemove()));
  connect(_addAll, SIGNAL(clicked()), this, SLOT(sAddAll()));
  connect(_removeAll, SIGNAL(clicked()), this, SLOT(sRemoveAll()));
  connect(_select, SIGNAL(clicked()), this, SLOT(sSelect()));
  connect(_cancel, SIGNAL(clicked()), this, SLOT(sCancel()));
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
  emit addClicked();
  foreach(XTreeWidgetItem *xtitem, _avail->selectedItems())
  {
    move(xtitem, true);
    emit itemAdded(xtitem->id());
  }
}

void SelectionWidget::sRemove()
{
  emit removeClicked();
  foreach(XTreeWidgetItem *xtitem, _sel->selectedItems())
  {
    move(xtitem, false);
    emit itemRemoved(xtitem->id());
  }
}

void SelectionWidget::sAddAll()
{
  emit addAllClicked();
  for (int i = 0; i < _avail->topLevelItemCount(); i++)
  {
    XTreeWidgetItem *xtitem = dynamic_cast<XTreeWidgetItem*>(_avail->takeTopLevelItem(i));
    move(xtitem, true);
    emit itemAdded(xtitem->id());
  }
}

void SelectionWidget::sRemoveAll()
{
  emit removeAllClicked();
  for (int i = 0; i < _sel->topLevelItemCount(); i++)
  {
    XTreeWidgetItem *xtitem = dynamic_cast<XTreeWidgetItem*>(_sel->takeTopLevelItem(i));
    move(xtitem, false);
    emit itemRemoved(xtitem->id());
  }
}


void SelectionWidget::move(XTreeWidgetItem *pXtitem, bool pAdd)
{
  XTreeWidget *sourceTree;
  XTreeWidget *destTree;
  QList<XTreeWidgetItem*> checkList;
  QList<XTreeWidgetItem*> appendList;

  if (pAdd)
  {
    sourceTree = _avail;
    destTree = _sel;
    checkList = _removed;
    appendList = _added;
  }
  else
  {
    sourceTree = _sel;
    destTree = _avail;
    checkList = _added;
    appendList = _removed;
  }
  
  sourceTree->takeTopLevelItem(sourceTree->indexOfTopLevelItem(pXtitem));
  if (!checkList.removeOne(pXtitem))
  {
    appendList.append(pXtitem);
  }
  destTree->addTopLevelItem(pXtitem);
}

void SelectionWidget::buildAddQuery()
{
  XSqlQuery qry;
  XSqlQuery rollback;
  rollback.prepare("ROLLBACK;");
  qry.exec("BEGIN;");

  foreach(XTreeWidgetItem *xtitem, _added)
  {
    QString qryString = "INSERT INTO ";
    qryString.append(_addTableName + " (");
    for (int i = 0; i < _addConstraints.count(); i++)
    {
      qryString.append(_addConstraints.name(i) + ", ");
    }
    qryString.chop(2);//remove the ", " from the last iteration.
    qryString.append(") VALUES (");
    for (int i = 0; i < _addConstraints.count(); i++)
    {
      qryString.append(":parameter");
      qryString.append(i);
      qryString.append(", ");
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
      qry.bindValue(":parameter" + i, bindVal);
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
      return;
    }
    
  }
  qry.exec("COMMIT;");
}

/*void SelectionWidget::buildRemoveQuery()
{
  QString qryString = "DELETE FROM ";
  qryString.append(_removeTableName + " WHERE ");
  if (! _removeByIdColName.isEmpty())
  {
    qryString.append(_removeByIdColName + " IN (");
    for (int i = 0; i < _removed.count(); i++)
    {
      qryString.append(":parameter" + i + ", ");
    }
    qryString.chop(2);//remove the ", " from the last iteration.
    qryString.append(");");
    qry.prepare(qryString);
    foreach(XTreeWidgetItem *xtitem, _removed)
    {
      if (_removeByAltId)
      {


    


  for (int i = 0; i < _addConstraints.count(); i++)
  {
    qryString.append(_addConstraints.name(i) + ", ")
  }
  qryString.chop(2);//remove the ", " from the last iteration.
  qryString.append(") VALUES (");
  for (int i = 0; i < _addConstraints.count(); i++)
  {
    qryString.append(":parameter" + i + ", ");
  }
  qryString.chop(2);//remove the ", " from the last iteration.
  qryString.append(");");

  if (DEBUG) { qDebug("Add query string: " + qryString); }
  */
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
