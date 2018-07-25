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
#include "qwebenginepageproto.h"

#include <QAction>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QPalette>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QUndoStack>
#include <QVariant>
#include <QWebFrame>
#include <QWebHistory>
#include <QWebEnginePage>
#include <QWebPluginFactory>
#include <QWebSettings>
#include <QWidget>


QScriptValue FeatureToScriptValue(QScriptEngine *engine, const QWebEnginePage::Feature &item)
{
  return engine->newVariant(item);
}
void FeatureFromScriptValue(const QScriptValue &obj, QWebEnginePage::Feature &item)
{
  item = (QWebEnginePage::Feature)obj.toInt32();
}

QScriptValue FindFlagToScriptValue(QScriptEngine *engine, const QWebEnginePage::FindFlag &item)
{
  return engine->newVariant(item);
}
void FindFlagFromScriptValue(const QScriptValue &obj, QWebEnginePage::FindFlag &item)
{
  item = (QWebEnginePage::FindFlag)obj.toInt32();
}

QScriptValue NavigationTypeToScriptValue(QScriptEngine *engine, const QWebEnginePage::NavigationType &item)
{
  return engine->newVariant(item);
}
void NavigationTypeFromScriptValue(const QScriptValue &obj, QWebEnginePage::NavigationType &item)
{
  item = (QWebEnginePage::NavigationType)obj.toInt32();
}

QScriptValue PermissionPolicyToScriptValue(QScriptEngine *engine, const QWebEnginePage::PermissionPolicy &item)
{
  return engine->newVariant(item);
}
void PermissionPolicyFromScriptValue(const QScriptValue &obj, QWebEnginePage::PermissionPolicy &item)
{
  item = (QWebEnginePage::PermissionPolicy)obj.toInt32();
}

QScriptValue WebActionToScriptValue(QScriptEngine *engine, const QWebEnginePage::WebAction &item)
{
  return engine->newVariant(item);
}
void WebActionFromScriptValue(const QScriptValue &obj, QWebEnginePage::WebAction &item)
{
  item = (QWebEnginePage::WebAction)obj.toInt32();
}

QScriptValue WebWindowTypeToScriptValue(QScriptEngine *engine, const QWebEnginePage::WebWindowType &item)
{
  return engine->newVariant(item);
}
void WebWindowTypeFromScriptValue(const QScriptValue &obj, QWebEnginePage::WebWindowType &item)
{
  item = (QWebEnginePage::WebWindowType)obj.toInt32();
}

QScriptValue QWebEnginePagetoScriptValue(QScriptEngine *engine, QWebEnginePage* const &item)
{
  return engine->newQObject(item);
}
void QWebEnginePagefromScriptValue(const QScriptValue &obj, QWebEnginePage* &item)
{
  item = qobject_cast<QWebEnginePage*>(obj.toQObject());
}

void setupQWebEnginePageProto(QScriptEngine *engine)
{
  qScriptRegisterMetaType(engine, QWebEnginePagetoScriptValue, QWebEnginePagefromScriptValue);
  QScriptValue::PropertyFlags permanent = QScriptValue::ReadOnly | QScriptValue::Undeletable;

  QScriptValue proto = engine->newQObject(new QWebEnginePageProto(engine));
  engine->setDefaultPrototype(qMetaTypeId<QWebEnginePage*>(), proto);

  QScriptValue constructor = engine->newFunction(constructQWebEnginePage, proto);
  engine->globalObject().setProperty("QWebEnginePage",  constructor);

  qScriptRegisterMetaType(engine, FeatureToScriptValue, FeatureFromScriptValue);
  constructor.setProperty("Notifications", QScriptValue(engine, QWebEnginePage::Notifications), permanent);
  constructor.setProperty("Geolocation", QScriptValue(engine, QWebEnginePage::Geolocation), permanent);

  qScriptRegisterMetaType(engine, FindFlagToScriptValue, FindFlagFromScriptValue);
  constructor.setProperty("FindBackward", QScriptValue(engine, QWebEnginePage::FindBackward), permanent);
  constructor.setProperty("FindCaseSensitively", QScriptValue(engine, QWebEnginePage::FindCaseSensitively), permanent);

  qScriptRegisterMetaType(engine, NavigationTypeToScriptValue, NavigationTypeFromScriptValue);
  constructor.setProperty("NavigationTypeLinkClicked", QScriptValue(engine, QWebEnginePage::NavigationTypeLinkClicked), permanent);
  constructor.setProperty("NavigationTypeFormSubmitted", QScriptValue(engine, QWebEnginePage::NavigationTypeFormSubmitted), permanent);
  constructor.setProperty("NavigationTypeReload", QScriptValue(engine, QWebEnginePage::NavigationTypeReload), permanent);
  constructor.setProperty("NavigationTypeOther", QScriptValue(engine, QWebEnginePage::NavigationTypeOther), permanent);

  qScriptRegisterMetaType(engine, PermissionPolicyToScriptValue, PermissionPolicyFromScriptValue);
  constructor.setProperty("PermissionUnknown", QScriptValue(engine, QWebEnginePage::PermissionUnknown), permanent);
  constructor.setProperty("PermissionGrantedByUser", QScriptValue(engine, QWebEnginePage::PermissionGrantedByUser), permanent);
  constructor.setProperty("PermissionDeniedByUser", QScriptValue(engine, QWebEnginePage::PermissionDeniedByUser), permanent);

  qScriptRegisterMetaType(engine, WebActionToScriptValue, WebActionFromScriptValue);
  constructor.setProperty("NoWebAction", QScriptValue(engine, QWebEnginePage::NoWebAction), permanent);
  constructor.setProperty("OpenLinkInNewWindow", QScriptValue(engine, QWebEnginePage::OpenLinkInNewWindow), permanent);
  constructor.setProperty("OpenLinkInThisWindow", QScriptValue(engine, QWebEnginePage::OpenLinkInThisWindow), permanent);
  constructor.setProperty("DownloadLinkToDisk", QScriptValue(engine, QWebEnginePage::DownloadLinkToDisk), permanent);
  constructor.setProperty("CopyLinkToClipboard", QScriptValue(engine, QWebEnginePage::CopyLinkToClipboard), permanent);
  constructor.setProperty("DownloadImageToDisk", QScriptValue(engine, QWebEnginePage::DownloadImageToDisk), permanent);
  constructor.setProperty("CopyImageToClipboard", QScriptValue(engine, QWebEnginePage::CopyImageToClipboard), permanent);
  constructor.setProperty("CopyImageUrlToClipboard", QScriptValue(engine, QWebEnginePage::CopyImageUrlToClipboard), permanent);
  constructor.setProperty("Back", QScriptValue(engine, QWebEnginePage::Back), permanent);
  constructor.setProperty("Forward", QScriptValue(engine, QWebEnginePage::Forward), permanent);
  constructor.setProperty("Stop", QScriptValue(engine, QWebEnginePage::Stop), permanent);
  constructor.setProperty("Reload", QScriptValue(engine, QWebEnginePage::Reload), permanent);
  constructor.setProperty("ReloadAndBypassCache", QScriptValue(engine, QWebEnginePage::ReloadAndBypassCache), permanent);
  constructor.setProperty("Cut", QScriptValue(engine, QWebEnginePage::Cut), permanent);
  constructor.setProperty("Copy", QScriptValue(engine, QWebEnginePage::Copy), permanent);
  constructor.setProperty("Paste", QScriptValue(engine, QWebEnginePage::Paste), permanent);
  constructor.setProperty("Undo", QScriptValue(engine, QWebEnginePage::Undo), permanent);
  constructor.setProperty("Redo", QScriptValue(engine, QWebEnginePage::Redo), permanent);
  constructor.setProperty("ToggleBold", QScriptValue(engine, QWebEnginePage::ToggleBold), permanent);
  constructor.setProperty("ToggleItalic", QScriptValue(engine, QWebEnginePage::ToggleItalic), permanent);
  constructor.setProperty("ToggleUnderline", QScriptValue(engine, QWebEnginePage::ToggleUnderline), permanent);
  constructor.setProperty("InspectElement", QScriptValue(engine, QWebEnginePage::InspectElement), permanent);
  constructor.setProperty("SelectAll", QScriptValue(engine, QWebEnginePage::SelectAll), permanent);
  constructor.setProperty("PasteAndMatchStyle", QScriptValue(engine, QWebEnginePage::PasteAndMatchStyle), permanent);
  constructor.setProperty("ToggleStrikethrough", QScriptValue(engine, QWebEnginePage::ToggleStrikethrough), permanent);
  constructor.setProperty("InsertUnorderedList", QScriptValue(engine, QWebEnginePage::InsertUnorderedList), permanent);
  constructor.setProperty("InsertOrderedList", QScriptValue(engine, QWebEnginePage::InsertOrderedList), permanent);
  constructor.setProperty("Indent", QScriptValue(engine, QWebEnginePage::Indent), permanent);
  constructor.setProperty("Outdent", QScriptValue(engine, QWebEnginePage::Outdent), permanent);
  constructor.setProperty("AlignCenter", QScriptValue(engine, QWebEnginePage::AlignCenter), permanent);
  constructor.setProperty("AlignJustified", QScriptValue(engine, QWebEnginePage::AlignJustified), permanent);
  constructor.setProperty("AlignLeft", QScriptValue(engine, QWebEnginePage::AlignLeft), permanent);
  constructor.setProperty("AlignRight", QScriptValue(engine, QWebEnginePage::AlignRight), permanent);
  constructor.setProperty("DownloadMediaToDisk", QScriptValue(engine, QWebEnginePage::DownloadMediaToDisk), permanent);
  constructor.setProperty("CopyMediaUrlToClipboard", QScriptValue(engine, QWebEnginePage::CopyMediaUrlToClipboard), permanent);
  constructor.setProperty("ToggleMediaControls", QScriptValue(engine, QWebEnginePage::ToggleMediaControls), permanent);
  constructor.setProperty("ToggleMediaLoop", QScriptValue(engine, QWebEnginePage::ToggleMediaLoop), permanent);
  constructor.setProperty("ToggleMediaPlayPause", QScriptValue(engine, QWebEnginePage::ToggleMediaPlayPause), permanent);
  constructor.setProperty("ToggleMediaMute", QScriptValue(engine, QWebEnginePage::ToggleMediaMute), permanent);

  qScriptRegisterMetaType(engine, WebWindowTypeToScriptValue, WebWindowTypeFromScriptValue);
  constructor.setProperty("WebBrowserWindow", QScriptValue(engine, QWebEnginePage::WebBrowserWindow), permanent);
}

QScriptValue constructQWebEnginePage(QScriptContext * context, QScriptEngine  *engine)
{
  QWebEnginePage *obj = 0;
  if (context->argumentCount() == 1)
    obj = new QWebEnginePage(qobject_cast<QWidget*>(context->argument(0).toQObject()));
  else
    obj = new QWebEnginePage();
  return engine->toScriptValue(obj);
}

QWebEnginePageProto::QWebEnginePageProto(QObject *parent)
    : QObject(parent)
{
}
QWebEnginePageProto::~QWebEnginePageProto()
{
}

QAction* QWebEnginePageProto::action(QWebEnginePage::WebAction action) const
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->action(action);
  return 0;
}

QMenu* QWebEnginePageProto::createStandardContextMenu()
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->createStandardContextMenu();
  return 0;
}

bool QWebEnginePageProto::hasSelection() const
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->hasSelection();
  return false;
}

QWebEngineHistory* QWebEnginePageProto::history() const
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->history();
  return 0;
}

QString QWebEnginePageProto::selectedText() const
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->selectedText();
  return QString();
}

void QWebEnginePageProto::setFeaturePermission(const QUrl &securityOrigin, QWebEnginePage::Feature feature, QWebEnginePage::PermissionPolicy policy)
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    item->setFeaturePermission(securityOrigin, feature, policy);
}

void QWebEnginePageProto::setView(QWidget * view)
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    item->setView(view);
}

QWebEngineSettings* QWebEnginePageProto::settings() const
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->settings();
  return 0;
}

void QWebEnginePageProto::triggerAction(QWebEnginePage::WebAction action, bool checked)
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    item->triggerAction(action, checked);
}

QWidget* QWebEnginePageProto::view() const
{
  QWebEnginePage *item = qscriptvalue_cast<QWebEnginePage*>(thisObject());
  if (item)
    return item->view();
  return 0;
}
