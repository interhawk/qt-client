/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef QSETTINGSPROTO_H
#define QSETTINGSPROTO_H

#include <QSettings>
#include <QObject>
#include <QtScript>

Q_DECLARE_METATYPE(QSettings*)
Q_DECLARE_METATYPE(enum QSettings::Format)
Q_DECLARE_METATYPE(enum QSettings::Scope)
Q_DECLARE_METATYPE(enum QSettings::Status)

void setupQSettingsProto(QScriptEngine *engine);
QScriptValue constructQSettings(QScriptContext *context, QScriptEngine *engine);

class QSettingsProto : public QObject, public QScriptable
{
  Q_OBJECT

  public:
    QSettingsProto(QObject *parent);

    Q_INVOKABLE QStringList         allKeys() const;
    Q_INVOKABLE QString             applicationName() const;
    Q_INVOKABLE void                beginGroup(const QString &prefix);
    Q_INVOKABLE int                 beginReadArray(const QString &prefix);
    Q_INVOKABLE void                beginWriteArray(const QString &prefix, int size = -1);
    Q_INVOKABLE QStringList         childGroups() const;
    Q_INVOKABLE QStringList         childKeys() const;
    Q_INVOKABLE void                clear();
    Q_INVOKABLE bool                contains(const QString &key) const;
    Q_INVOKABLE void                endArray();
    Q_INVOKABLE void                endGroup();
    Q_INVOKABLE bool                fallbacksEnabled() const;
    Q_INVOKABLE QString             fileName() const;
    Q_INVOKABLE QSettings::Format   format() const;
    Q_INVOKABLE QString             group() const;
    Q_INVOKABLE bool                isWritable() const;
    Q_INVOKABLE QString             organizationName() const;
    Q_INVOKABLE void                remove(const QString &key);
    Q_INVOKABLE QSettings::Scope    scope() const;
    Q_INVOKABLE void                setArrayIndex(int i);
    Q_INVOKABLE void                setFallbacksEnabled(bool b);
    Q_INVOKABLE void                setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE QSettings::Status   status() const;
    Q_INVOKABLE void                sync();
    Q_INVOKABLE QVariant            value(const QString &key, const QVariant &defaultValue = QVariant()) const;

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    Q_INVOKABLE bool                isAtomicSyncRequired() const;
    Q_INVOKABLE void                setAtomicSyncRequired(bool enable);
#endif
};

#endif
