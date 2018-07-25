/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QWEBENGINEPAGEPROTO_H__
#define __QWEBENGINEPAGEPROTO_H__

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QObject>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QtScript>
#include <QUndoStack>
#include <QVariant>
#include <QWebEnginePage>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QWidget>

Q_DECLARE_METATYPE(QWebEnginePage*)

Q_DECLARE_METATYPE(enum QWebEnginePage::Feature)
Q_DECLARE_METATYPE(enum QWebEnginePage::FindFlag)
Q_DECLARE_METATYPE(enum QWebEnginePage::NavigationType)
Q_DECLARE_METATYPE(enum QWebEnginePage::PermissionPolicy)
Q_DECLARE_METATYPE(enum QWebEnginePage::WebAction)
Q_DECLARE_METATYPE(enum QWebEnginePage::WebWindowType)

void setupQWebEnginePageProto(QScriptEngine *engine);
QScriptValue constructQWebEnginePage(QScriptContext *context, QScriptEngine *engine);

class QWebEnginePageProto : public QObject, public QScriptable
{
  Q_OBJECT

  Q_PROPERTY (const bool hasSelection                               READ hasSelection)
  Q_PROPERTY (const QString selectedText                            READ selectedText)

  public:
    QWebEnginePageProto(QObject *parent);
    ~QWebEnginePageProto();

    Q_INVOKABLE QAction                        *action(QWebEnginePage::WebAction action) const;
    Q_INVOKABLE QMenu                          *createStandardContextMenu();
    Q_INVOKABLE bool                            hasSelection() const;
    Q_INVOKABLE QWebEngineHistory              *history() const;
    Q_INVOKABLE QString                         selectedText() const;
    Q_INVOKABLE void                            setFeaturePermission(const QUrl &securityOrigin, QWebEnginePage::Feature feature, QWebEnginePage::PermissionPolicy policy);
    Q_INVOKABLE void                            setView(QWidget * view);
    Q_INVOKABLE QWebEngineSettings             *settings() const;
    Q_INVOKABLE virtual void                    triggerAction(QWebEnginePage::WebAction action, bool checked = false);
    Q_INVOKABLE QWidget                        *view() const;

  signals:
    void    contentsChanged();
    void    downloadRequested(const QNetworkRequest & request);
    void    geometryChangeRequested(const QRect & geom);
    void    linkClicked(const QUrl & url);
    void    linkHovered(const QString & link, const QString & title, const QString & textContent);
    void    loadFinished(bool ok);
    void    loadProgress(int progress);
    void    loadStarted();
    void    menuBarVisibilityChangeRequested(bool visible);
    void    microFocusChanged();
    void    repaintRequested(const QRect & dirtyRect);
    void    scrollRequested(int dx, int dy, const QRect & rectToScroll);
    void    selectionChanged();
    void    statusBarMessage(const QString & text);
    void    statusBarVisibilityChangeRequested(bool visible);
    void    toolBarVisibilityChangeRequested(bool visible);
    void    unsupportedContent(QNetworkReply * reply);
    void    viewportChangeRequested();
    void    windowCloseRequested();

};

#endif
