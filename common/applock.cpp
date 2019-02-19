/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "applock.h"

#include <QtScript>
#include <QMessageBox>
#include <QSqlError>
#include <QVariant>
#include <QWidget>

#include "errorReporter.h"
#include "xsqlquery.h"

class AppLockPrivate
{
  public:
    AppLockPrivate(AppLock *parent)
      : _id(-1),
        _myLock(false),
        _otherLock(false),
        _parent(parent)
    {
      _dbIsOpen = QSqlDatabase::database().isOpen();
      if (! _dbIsOpen)
        return;

      if (_isWebEnabled.isNull())
      {
        XSqlQuery q("SELECT EXISTS(SELECT 1"
                    "  FROM pg_class c"
                    "  JOIN pg_namespace n ON (relnamespace = n.oid)"
                    " WHERE relname = 'lock'"
                    "   AND nspname = 'xt') AS mobilized;");
        if (q.first())
          _isWebEnabled = q.value("mobilized");
        else
          (void)ErrorReporter::error(QtCriticalMsg,
                                     qobject_cast<QWidget*>(parent->parent()),
                                     parent->tr("Locking Error"),
                                     q, __FILE__, __LINE__);
      }

      updateLockStatus();
    }

    void updateLockStatus()
    {
      if (! _dbIsOpen || _id < 0)
        return;

      XSqlQuery q;
      if (_isWebEnabled.toBool())
        q.prepare("SELECT lock_pid = pg_backend_pid() AS mylock, lock_username"
                  "  FROM xt.lock"
                  "  JOIN pg_class c ON lock_table_oid = c.oid"
                  " WHERE relname = :table"
                  "   AND lock_record_id = :id;");
      else
        q.prepare("SELECT l.pid = pg_backend_pid() AS mylock, usename AS lock_username"
                  "  FROM pg_locks l"
                  "  JOIN pg_class c    on classid = c.oid"
                  "  JOIN pg_database d on database = d.oid"
                  "  JOIN pg_stat_activity a ON l.pid = a.pid"
                  " WHERE d.datname = current_database()"
                  "   AND relname = :table"
                  "   AND objid   = :id"
                  "   AND locktype = 'advisory';");

      q.bindValue(":table", _table);
      q.bindValue(":id",    _id);
      q.exec();
      if (q.first())
      {
        _myLock    = q.value("mylock").toBool();
        _otherLock = ! _myLock;
        _username  = q.value("lock_username").toString();
        return;
      }
      else if (ErrorReporter::error(QtCriticalMsg,
                                    qobject_cast<QWidget*>(_parent->parent()),
                                    _parent->tr("Locking Error"),
                                    q, __FILE__, __LINE__))
        return;
      else
      {
        _myLock = _otherLock = false;
        _username.clear();
      }
    }

    QString  _error;
    int      _id;
    bool     _dbIsOpen;
    bool     _myLock;
    bool     _otherLock;
    AppLock *_parent;
    QString  _table;
    QString  _username;

    static QVariant _isWebEnabled;
};

QVariant AppLockPrivate::_isWebEnabled;

AppLock::AppLock(QObject *parent)
  : QObject(parent)
{
  _p = new AppLockPrivate(this);
}

AppLock::AppLock(QString table, int id, QObject *parent)
  : QObject(parent)
{
  _p = new AppLockPrivate(this);
  (void)acquire(table, id);
}

AppLock::~AppLock()
{
  (void)release();
  delete _p;
  _p = 0;
}

/** Try to acquire an application-level lock on the record described in the
    constructor or last call to this AppLock's acquire(table, id).

   @return true if this AppLock instance appears to hold the lock
   @return false if the lock could not be acquired

   @see holdsLock()
   @see isLockedOut()
   @see lastError()
 */
bool AppLock::acquire(AppLock::AcquireMode mode)
{
  if (! _p->_dbIsOpen)
    return false;

  if (_p->_id < 0 || _p->_table.isEmpty())
  {
    _p->_error = tr("Cannot acquire a lock without a table and record id.");
    if (mode == Interactive)
      QMessageBox::critical(0, tr("Cannot Acquire Lock"), _p->_error);
    return false;
  }

  bool result = false;
  _p->_error.clear();
  XSqlQuery q;
  q.prepare("SELECT tryLock(CAST(oid AS INTEGER), :id) AS locked"
            "  FROM pg_class"
            " WHERE relname=:table;");
  q.bindValue(":id",    _p->_id);
  q.bindValue(":table", _p->_table);
  q.exec();
  if (q.first())
  {
    result = q.value("locked").toBool();
    if (result)
    {
      _p->_myLock    = true;
      _p->_otherLock = false;
      _p->_username.clear();
      result = true;
    }
    else
    {
      _p->updateLockStatus();
      result = _p->_myLock;
      if (_p->_otherLock)
      {
        _p->_error = tr("The record you are trying to edit is currently being "
                         "edited by another user (%1).").arg(_p->_username);
        if (mode == Interactive)
          QMessageBox::critical(0, tr("Cannot Acquire Lock"), _p->_error);
      }
    }
  }
  else if (ErrorReporter::error(QtCriticalMsg, qobject_cast<QWidget*>(parent()),
                                tr("Locking Error"),
                                q, __FILE__, __LINE__))
    _p->_error = q.lastError().databaseText();

  return result;
}

bool AppLock::acquire(QString table, int id, AppLock::AcquireMode mode)
{
  if (_p->_myLock && id != _p->_id && table != _p->_table)
  {
    _p->_error = tr("Cannot change the description of a locked object.");
    return false;
  }
  _p->_table = table;
  _p->_id    = id;

  return acquire(mode);
}

/** @return true if _this_ instance of AppLock holds the lock */
bool AppLock::holdsLock() const
{
  return _p->_myLock;
}

/** @return true if the object appears locked by some other entity */
bool AppLock::isLockedOut() const
{
  _p->updateLockStatus();
  return _p->_otherLock;
}

// we don't care _how_ the lock gets cleared, just _that_ it gets cleared
bool AppLock::release()
{
  if (_p->_id < 0 || _p->_table.isEmpty() || ! _p->_dbIsOpen)
    return true;

  _p->_error.clear();
  if (_p->_myLock)
  {
    XSqlQuery q;
    q.prepare("SELECT pg_advisory_unlock(CAST(oid AS INTEGER), :id)"
              "  FROM pg_class"
              " WHERE relname = :table;");
    q.bindValue(":id",    _p->_id);
    q.bindValue(":table", _p->_table);
    q.exec();
    if (q.first())
    {
      _p->_myLock    = false;
      _p->_otherLock = false;
    }
    else if (ErrorReporter::error(QtCriticalMsg, qobject_cast<QWidget*>(parent()),
                                  tr("Unlocking Error"),
                                  q, __FILE__, __LINE__))
    {
      _p->_error = q.lastError().text();
      return false;
    }
  }
  return true;
}

QString AppLock::lastError() const
{
  return _p->_error;
}

QString AppLock::toString()  const
{
  QString result("AppLock[%1, %2 (%3)]");
  return result.arg(_p->_table).arg(_p->_id).arg(_p->_error);
}

// script exposure /////////////////////////////////////////////////////////////

QScriptValue AppLockToScriptValue(QScriptEngine *engine, AppLock *const &lock)
{
  return engine->newQObject(lock);
}

void AppLockFromScriptValue(const QScriptValue &obj, AppLock * &lock)
{
  lock = qobject_cast<AppLock *>(obj.toQObject());
}

void setupAppLockProto(QScriptEngine *engine)
{
  qScriptRegisterMetaType(engine, AppLockToScriptValue, AppLockFromScriptValue);

  QScriptValue proto = engine->newQObject(new AppLockProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<AppLock*>(), proto);

  QScriptValue constructor = engine->newFunction(constructAppLock, proto);
  engine->globalObject().setProperty("AppLock", constructor);

  constructor.setProperty("Silent",      QScriptValue(engine, AppLock::Silent),      QScriptValue::ReadOnly | QScriptValue::Undeletable);
  constructor.setProperty("Interactive", QScriptValue(engine, AppLock::Interactive), QScriptValue::ReadOnly | QScriptValue::Undeletable);
}

QScriptValue constructAppLock(QScriptContext *context, QScriptEngine *engine)
{
  AppLock *lock = 0;

  if (context->argumentCount() == 0)
    lock = new AppLock();
  else if (context->argumentCount() == 1)
    lock = new AppLock(qscriptvalue_cast<QObject*>(context->argument(0)));
  else
  {
    QVariant table  = context->argument(0).toVariant();
    QVariant id     = context->argument(1).toVariant();
    QObject *parent = context->argument(2).toQObject();

    if (table.isValid() && id.isValid())
      lock = new AppLock(table.toString(), id.toInt(), parent);
    else
      context->throwError(QScriptContext::UnknownError,
                          QString("Could not find an appropriate AppLock constructor"));
  }

  return engine->toScriptValue(lock);
}

AppLockProto::AppLockProto(QObject *parent)
  : QObject(parent)
{
}

bool AppLockProto::acquire(AppLock::AcquireMode mode)
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->acquire(mode);
  return false;
}

bool AppLockProto::acquire(QString table, int id, enum AppLock::AcquireMode mode)
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->acquire(table, id, mode);
  return false;
}

bool AppLockProto::holdsLock() const
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->holdsLock();
  return false;
}

bool AppLockProto::isLockedOut() const
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->isLockedOut();
  return false;
}

QString AppLockProto::lastError() const
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->lastError();
  return QString(tr("This object does not appear to be an application lock."));
}

bool AppLockProto::release()
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->release();
  return false;
}

QString AppLockProto::toString() const
{
  AppLock *lock = qscriptvalue_cast<AppLock*>(thisObject());
  if (lock)
    return lock->toString();
  return QString();
}

