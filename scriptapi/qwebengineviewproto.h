/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2018 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which(including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QWEBENGINEVIEWPROTO_H__
#define __QWEBENGINEVIEWPROTO_H__

#include <QByteArray>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
#include <QPainter>
#include <QPrinter>
#include <QString>
#include <QtScript>
#include <QUrl>
#include <QWebEngineView>
#include <QWebEnginePage>

Q_DECLARE_METATYPE(QWebEngineView*)

void setupQWebEngineViewProto(QScriptEngine *engine);
QScriptValue constructQWebEngineView(QScriptContext *context, QScriptEngine *engine);

class QWebEngineViewProto : public QObject, public QScriptable
{
  Q_OBJECT

  Q_PROPERTY (const bool hasSelection               READ hasSelection)
  Q_PROPERTY (const QIcon icon                      READ icon)
  Q_PROPERTY (const QString selectedText            READ selectedText)
  Q_PROPERTY (const QString title                   READ title)
  Q_PROPERTY (QUrl url                              READ url            WRITE setUrl)

  public:
    QWebEngineViewProto(QObject *parent);
    virtual ~QWebEngineViewProto();

    Q_INVOKABLE void                    findText(const QString &subString, QWebEnginePage::FindFlags options = 0);
    Q_INVOKABLE bool                    hasSelection() const;
    Q_INVOKABLE QWebEngineHistory      *history() const;
    Q_INVOKABLE QIcon                   icon() const;
    Q_INVOKABLE void                    load(const QUrl &url);
    Q_INVOKABLE QWebEnginePage         *page() const;
    Q_INVOKABLE QAction                *pageAction(QWebEnginePage::WebAction action) const;
    Q_INVOKABLE QString                 selectedText() const;
    Q_INVOKABLE void                    setContent(const QByteArray &data, const QString &mimeType = QString(), const QUrl &baseUrl = QUrl());
    Q_INVOKABLE void                    setHtml(const QString &html, const QUrl &baseUrl = QUrl());
    Q_INVOKABLE void                    setPage(QWebEnginePage *page);
    Q_INVOKABLE QWebEngineSettings     *settings() const;
    Q_INVOKABLE void                    setUrl(const QUrl & url);
    Q_INVOKABLE void                    setZoomFactor(qreal factor);
    Q_INVOKABLE QString                 title() const;
    Q_INVOKABLE void                    triggerPageAction(QWebEnginePage::WebAction action, bool checked = false);
    Q_INVOKABLE QUrl                    url() const;
    Q_INVOKABLE qreal                   zoomFactor() const;

  // Reimplemented Public Functions.
    Q_INVOKABLE QVariant                inputMethodQuery(Qt::InputMethodQuery property) const;
    Q_INVOKABLE QSize                   sizeHint() const;

  public Q_SLOTS:
    Q_INVOKABLE void                    back();
    Q_INVOKABLE void                    forward();
    Q_INVOKABLE void                    reload();
    Q_INVOKABLE void                    stop();

  signals:
    void    iconChanged();
    void    linkClicked(const QUrl & url);
    void    loadFinished(bool ok);
    void    loadProgress(int progress);
    void    loadStarted();
    void    selectionChanged();
    void    statusBarMessage(const QString & text);
    void    titleChanged(const QString & title);
    void    urlChanged(const QUrl & url);

};

#endif
