/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "scriptapi_internal.h"
#include "qsettings.h"

QScriptValue FormattoScriptValue(QScriptEngine *engine, const enum QSettings::Format &p)
{
  return QScriptValue(engine, (int)p);
}

void FormatfromScriptValue(const QScriptValue &obj, enum QSettings::Format &p)
{
  p = (enum QSettings::Format)obj.toInt32();
}

QScriptValue ScopetoScriptValue(QScriptEngine *engine, const enum QSettings::Scope &p)
{
  return QScriptValue(engine, (int)p);
}

void ScopefromScriptValue(const QScriptValue &obj, enum QSettings::Scope &p)
{
  p = (enum QSettings::Scope)obj.toInt32();
}

QScriptValue StatustoScriptValue(QScriptEngine *engine, const enum QSettings::Status &p)
{
  return QScriptValue(engine, (int)p);
}

void StatusfromScriptValue(const QScriptValue &obj, enum QSettings::Status &p)
{
  p = (enum QSettings::Status)obj.toInt32();
}

void setupQSettingsProto(QScriptEngine *engine)
{
  QScriptValue proto = engine->newQObject(new QSettingsProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<QSettings*>(), proto);

  QScriptValue constructor = engine->newFunction(constructQSettings, proto);
  engine->globalObject().setProperty("QSettings", constructor);

  qScriptRegisterMetaType(engine, FormattoScriptValue, FormatfromScriptValue);
  constructor.setProperty("NativeFormat",     QScriptValue(engine, QSettings::NativeFormat),      ENUMPROPFLAGS);
#if QT_VERSION >= QT_VERSION_CHECK(5,7,0) && defined(Q_OS_WIN)
  constructor.setProperty("Registry32Format", QScriptValue(engine, QSettings::Registry32Format),  ENUMPROPFLAGS);
  constructor.setProperty("Registry64Format", QScriptValue(engine, QSettings::Registry64Format),  ENUMPROPFLAGS);
#endif
  constructor.setProperty("IniFormat",        QScriptValue(engine, QSettings::IniFormat),         ENUMPROPFLAGS);
  constructor.setProperty("NativeFormat",     QScriptValue(engine, QSettings::NativeFormat),      ENUMPROPFLAGS);
  constructor.setProperty("InvalidFormat",    QScriptValue(engine, QSettings::InvalidFormat),     ENUMPROPFLAGS);

  qScriptRegisterMetaType(engine, ScopetoScriptValue, ScopefromScriptValue);
  constructor.setProperty("UserScope",   QScriptValue(engine, QSettings::UserScope),   ENUMPROPFLAGS);
  constructor.setProperty("SystemScope", QScriptValue(engine, QSettings::SystemScope), ENUMPROPFLAGS);

  qScriptRegisterMetaType(engine, StatustoScriptValue, StatusfromScriptValue);
  constructor.setProperty("NoError",     QScriptValue(engine, QSettings::NoError),     ENUMPROPFLAGS);
  constructor.setProperty("AccessError", QScriptValue(engine, QSettings::AccessError), ENUMPROPFLAGS);
  constructor.setProperty("FormatError", QScriptValue(engine, QSettings::FormatError), ENUMPROPFLAGS);

}

QScriptValue constructQSettings(QScriptContext *context, QScriptEngine *engine)
{
  QSettings *obj = 0;

  switch (context->argumentCount()) {
    case 0:
      // QSettings(QObject *parent = nullptr)
      obj = new QSettings();
      break;
    case 1:
      // QSettings(const QString &organization, const QString &application = QString(), QObject *parent = nullptr)
      if (context->argument(0).isString())
        obj = new QSettings(context->argument(0).toString());
      // QSettings(QObject *parent = nullptr)
      else if (context->argument(0).isObject())
        obj = new QSettings(qobject_cast<QObject*>(context->argument(0).toQObject()));
      break;
    case 2:
      // QSettings(const QString &fileName, QSettings::Format format, QObject *parent = nullptr)
      if (context->argument(0).isString() && context->argument(1).isNumber())
      {
        obj = new QSettings(context->argument(0).toString(),
                            qscriptvalue_cast<QSettings::Format>(context->argument(1).toInt32()));
      }
      // QSettings(QSettings::Scope scope, const QString &organization, const QString &application = QString(),
      //           QObject *parent = nullptr)
      else if (context->argument(0).isNumber() && context->argument(1).isString())
      {
        obj = new QSettings(qscriptvalue_cast<QSettings::Scope>(context->argument(1).toInt32()),
                           context->argument(0).toString());
      }
      // QSettings(const QString &organization, const QString &application = QString(), QObject *parent = nullptr)
      else if (context->argument(0).isString() && context->argument(1).isString())
      {
        obj = new QSettings(context->argument(0).toString(),
                            context->argument(1).toString());
      }
      break;
    case 3:
      //  QSettings(const QString &organization, const QString &application = QString(), QObject *parent = nullptr)
      if (context->argument(0).isString() &&
          context->argument(1).isString() &&
          context->argument(2).isObject())
      {
        obj = new QSettings(context->argument(0).toString(),
                            context->argument(1).toString(),
                            qobject_cast<QObject*>(context->argument(2).toQObject()));
      }
      // QSettings(const QString &fileName, QSettings::Format format, QObject *parent = nullptr)
      else if (context->argument(0).isString() &&
               context->argument(1).isNumber() &&
               context->argument(2).isObject())
      {
        obj = new QSettings(context->argument(0).toString(),
                            qscriptvalue_cast<QSettings::Format>(context->argument(1).toInt32()),
                            qobject_cast<QObject*>(context->argument(2).toQObject()));
      }
      // QSettings(QSettings::Scope scope, const QString &organization, const QString &application = QString(),
      //           QObject *parent = nullptr)
      else if (context->argument(0).isNumber() &&
               context->argument(1).isString() &&
               context->argument(2).isString())
      {
        obj = new QSettings(qscriptvalue_cast<QSettings::Scope>(context->argument(0).toInt32()),
                            context->argument(1).toString(),
                            context->argument(2).toString());
      }
      // QSettings(QSettings::Format format, QSettings::Scope scope, const QString &organization,
      //           const QString &application = QString(), QObject *parent = nullptr)
      else if (context->argument(0).isNumber() &&
               context->argument(1).isNumber() &&
               context->argument(2).isString())
      {
        obj = new QSettings(qscriptvalue_cast<QSettings::Format>(context->argument(0).toInt32()),
                            qscriptvalue_cast<QSettings::Scope>(context->argument(1).toInt32()),
                            context->argument(2).toString());
      }
      break;
    case 4:
      // QSettings(QSettings::Scope scope, const QString &organization, const QString &application = QString(),
      //           QObject *parent = nullptr)
      if (context->argument(0).isNumber() &&
          context->argument(1).isString() &&
          context->argument(2).isString() &&
          context->argument(3).isObject())
      {
        obj = new QSettings(qscriptvalue_cast<QSettings::Scope>(context->argument(0).toInt32()),
                            context->argument(1).toString(),
                            context->argument(2).toString(),
                            qobject_cast<QObject*>(context->argument(3).toQObject()));
      }
      // QSettings(QSettings::Format format, QSettings::Scope scope, const QString &organization,
      //           const QString &application = QString(), QObject *parent = nullptr)
      else if (context->argument(0).isNumber() &&
               context->argument(1).isNumber() &&
               context->argument(2).isString() &&
               context->argument(3).isString())
       {
         obj = new QSettings(qscriptvalue_cast<QSettings::Format>(context->argument(0).toInt32()),
                             qscriptvalue_cast<QSettings::Scope>(context->argument(1).toInt32()),
                             context->argument(2).toString(),
                             context->argument(3).toString());
      }
      break;
    case 5:
      // QSettings(QSettings::Format format, QSettings::Scope scope, const QString &organization,
      //           const QString &application = QString(), QObject *parent = nullptr)
      if (context->argument(0).isNumber() &&
          context->argument(1).isNumber() &&
          context->argument(2).isString() &&
          context->argument(3).isString() &&
          context->argument(4).isObject())
      {
        obj = new QSettings(qscriptvalue_cast<QSettings::Format>(context->argument(0).toInt32()),
                            qscriptvalue_cast<QSettings::Scope>(context->argument(1).toInt32()),
                            context->argument(2).toString(),
                            context->argument(3).toString(),
                            qobject_cast<QObject*>(context->argument(4).toQObject()));
      }
      break;
    default:
      break;
  }

  if (!obj)
    context->throwError(QScriptContext::UnknownError, "Could not find an appropriate QSettings constructor.");

  return engine->newQObject(obj);

}

QSettingsProto::QSettingsProto(QObject *parent) : QObject(parent)
{
}

QStringList QSettingsProto::allKeys() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->allKeys();
  return QStringList();
}

QString QSettingsProto::applicationName() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->applicationName();
  return QString();
}

void QSettingsProto::beginGroup(const QString &prefix)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->beginGroup(prefix);
}

int QSettingsProto::beginReadArray(const QString &prefix)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->beginReadArray(prefix);
  return 0;
}

void QSettingsProto::beginWriteArray(const QString &prefix, int size)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->beginWriteArray(prefix, size);
}

QStringList QSettingsProto::childGroups() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->childGroups();
  return QStringList();
}

QStringList QSettingsProto::childKeys() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->childKeys();
  return QStringList();
}

void QSettingsProto::clear()
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->clear();
}

bool QSettingsProto::contains(const QString &key) const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->contains(key);
  return false;
}

void QSettingsProto::endArray()
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->endArray();
}

void QSettingsProto::endGroup()
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->endGroup();
}

bool QSettingsProto::fallbacksEnabled() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->fallbacksEnabled();
  return false;
}

QString QSettingsProto::fileName() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->fileName();
  return QString();
}

QSettings::Format QSettingsProto::format() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->format();
  return QSettings::NativeFormat; // the default for each OS
}

QString QSettingsProto::group() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->group();
  return QString();
}

bool QSettingsProto::isWritable() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->isWritable();
  return false;
}

QString QSettingsProto::organizationName() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->organizationName();
  return QString();
}

void QSettingsProto::remove(const QString &key)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->remove(key);
}

QSettings::Scope QSettingsProto::scope() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->scope();
  return QSettings::UserScope;
}

void QSettingsProto::setArrayIndex(int i)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->setArrayIndex(i);
}

void QSettingsProto::setFallbacksEnabled(bool b)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->setFallbacksEnabled(b);
}

void QSettingsProto::setValue(const QString &key, const QVariant &value)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->setValue(key, value);
}

QSettings::Status QSettingsProto::status() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->status();
  return QSettings::NoError;
}

void QSettingsProto::sync()
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->sync();
}

QVariant QSettingsProto::value(const QString &key, const QVariant &defaultvalue) const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->value(key, defaultvalue);
  return QVariant();
}

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
bool QSettingsProto::isAtomicSyncRequired() const
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    return item->isAtomicSyncRequired();
  return false;
}

void QSettingsProto::setAtomicSyncRequired(bool enable)
{
  QSettings *item = qscriptvalue_cast<QSettings*>(thisObject());
  if (item)
    item->setAtomicSyncRequired(enable);
}
#endif
