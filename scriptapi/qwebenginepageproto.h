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
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QSizeF>
#include <QStringList>
#include <QtScript>
#include <QUndoStack>
#include <QVariant>
#include <QWebEngineContextMenuData>
#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>
#include <QWebEngineQuotaRequest>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QWidget>

Q_DECLARE_METATYPE(QWebEnginePage*)

Q_DECLARE_METATYPE(enum QWebEnginePage::Feature)
Q_DECLARE_METATYPE(enum QWebEnginePage::FileSelectionMode)
Q_DECLARE_METATYPE(enum QWebEnginePage::FindFlag)
Q_DECLARE_METATYPE(enum QWebEnginePage::JavaScriptConsoleMessageLevel)
Q_DECLARE_METATYPE(enum QWebEnginePage::NavigationType)
Q_DECLARE_METATYPE(enum QWebEnginePage::PermissionPolicy)
Q_DECLARE_METATYPE(enum QWebEnginePage::RenderProcessTerminationStatus)
Q_DECLARE_METATYPE(enum QWebEnginePage::WebAction)
Q_DECLARE_METATYPE(enum QWebEnginePage::WebWindowType)

void setupQWebEnginePageProto(QScriptEngine *engine);
QScriptValue constructQWebEnginePage(QScriptContext *context, QScriptEngine *engine);

class QWebEnginePageProto : public QObject, public QScriptable
{
  Q_OBJECT

  Q_PROPERTY (const bool audioMuted                 READ isAudioMuted)
  Q_PROPERTY (const QSizeF contentsSize             READ contentsSize)
  Q_PROPERTY (const bool hasSelection               READ hasSelection)
  Q_PROPERTY (const QIcon icon                      READ icon)
  Q_PROPERTY (const QUrl  iconUrl                   READ iconUrl)
  Q_PROPERTY (const bool recentlyAudible            READ recentlyAudible)
  Q_PROPERTY (const QUrl requestedUrl               READ requestedUrl)
  Q_PROPERTY (const QPointF scrollPosition          READ scrollPosition)
  Q_PROPERTY (const QString selectedText            READ selectedText)
  Q_PROPERTY (const QString title                   READ title)
  Q_PROPERTY (const QUrl  url                       READ url)
  Q_PROPERTY (const qreal zoomFactor                READ zoomFactor)

  public:
    QWebEnginePageProto(QObject *parent);
    ~QWebEnginePageProto();

    Q_INVOKABLE QAction                        *action(QWebEnginePage::WebAction action) const;
    Q_INVOKABLE QColor                          backgroundColor() const;
    Q_INVOKABLE QSizeF                          contentsSize() const;
    Q_INVOKABLE QMenu                          *createStandardContextMenu();
    Q_INVOKABLE QWebEnginePage                 *devToolsPage() const;
    Q_INVOKABLE void                            download(const QUrl &url, const QString &filename = QString());
    Q_INVOKABLE bool                            hasSelection() const;
    Q_INVOKABLE QWebEngineHistory              *history() const;
    Q_INVOKABLE QIcon                           icon() const;
    Q_INVOKABLE QUrl                            iconUrl() const;
    Q_INVOKABLE QWebEnginePage                 *inspectedPage() const;
    Q_INVOKABLE bool                            isAudioMuted() const;
    Q_INVOKABLE void                            load(const QUrl &url);
    //Q_INVOKABLE void                          load(const QWebEngineHttpRequest &request); TODO: Expose QWebEngineHttpRequest
    Q_INVOKABLE void                            print(QPrinter *printer, const QWebEngineCallback<bool> &resultCallback);
    Q_INVOKABLE void                            printToPdf(const QString &filePath, const QPageLayout &pageLayout = QPageLayout(QPageSize(QPageSize::Letter), QPageLayout::Portrait, QMarginsF()));
    Q_INVOKABLE void                            printToPdf(const QWebEngineCallback<const QByteArray &> &resultCallback, const QPageLayout &pageLayout = QPageLayout(QPageSize(QPageSize::Letter), QPageLayout::Portrait, QMarginsF()));
    Q_INVOKABLE QWebEngineProfile              *profile() const;
    Q_INVOKABLE bool                            recentlyAudible() const;
    Q_INVOKABLE void 	                        replaceMisspelledWord(const QString &replacement);
    Q_INVOKABLE QUrl                            requestedUrl() const;
    Q_INVOKABLE void                            runJavaScript(const QString &scriptSource);
    Q_INVOKABLE void                            runJavaScript(const QString &scriptSource, quint32 worldId);
    Q_INVOKABLE void                            runJavaScript(const QString &scriptSource, const QWebEngineCallback<const QVariant &> &resultCallback);
    Q_INVOKABLE void                            runJavaScript(const QString &scriptSource, quint32 worldId, const QWebEngineCallback<const QVariant &> &resultCallback);
    Q_INVOKABLE void                            save(const QString &filePath, QWebEngineDownloadItem::SavePageFormat format = QWebEngineDownloadItem::MimeHtmlSaveFormat) const;
    Q_INVOKABLE QPointF                         scrollPosition() const;
    Q_INVOKABLE QString                         selectedText() const;
    Q_INVOKABLE void                            setAudioMuted(bool muted);
    Q_INVOKABLE void                            setBackgroundColor(const QColor &color);
    Q_INVOKABLE void                            setContent(const QByteArray &data, const QString &mimeType = QString(), const QUrl &baseUrl = QUrl());
    Q_INVOKABLE void                            setDevToolsPage(QWebEnginePage *devToolsPage);
    Q_INVOKABLE void                            setFeaturePermission(const QUrl &securityOrigin, QWebEnginePage::Feature feature, QWebEnginePage::PermissionPolicy policy);
    Q_INVOKABLE void                            setHtml(const QString &html, const QUrl &baseUrl = QUrl());
    Q_INVOKABLE void                            setInspectedPage(QWebEnginePage *page);
    Q_INVOKABLE void                            setUrl(const QUrl &url);
    Q_INVOKABLE void                            setView(QWidget * view);
    Q_INVOKABLE void                            setWebChannel(QWebChannel *channel, uint worldId);
    Q_INVOKABLE void                            setWebChannel(QWebChannel *channel);
    Q_INVOKABLE void                            setZoomFactor(qreal factor);
    Q_INVOKABLE QWebEngineSettings             *settings() const;
    Q_INVOKABLE QString 	                    title() const;
    Q_INVOKABLE void                            toHtml(const QWebEngineCallback<const QString &> &resultCallback) const;
    Q_INVOKABLE void                            toPlainText(const QWebEngineCallback<const QString &> &resultCallback) const;
    Q_INVOKABLE virtual void                    triggerAction(QWebEnginePage::WebAction action, bool checked = false);
    Q_INVOKABLE QUrl                            url() const;
    Q_INVOKABLE QWidget                        *view() const;
    Q_INVOKABLE QWebChannel                    *webChannel() const;
    Q_INVOKABLE qreal                           zoomFactor() const;

    //Reimplemented Public Functions
    virtual bool 	event(QEvent *e);

  signals:
    void    audioMutedChanged(bool muted);
    void    authenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator);
    void    contentsSizeChanged(const QSizeF &size);
    void    featurePermissionRequestCanceled(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
    void    featurePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
    void    fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);
    void    geometryChangeRequested(const QRect &geom);
    void    iconChanged(const QIcon &icon);
    void    iconUrlChanged(const QUrl &url);
    void    linkHovered(const QString &url);
    void    loadFinished(bool ok);
    void    loadProgress(int progress);
    void    loadStarted();
    void    pdfPrintingFinished(const QString &filePath, bool success);
    void    proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator, const QString &proxyHost);
    void    quotaRequested(QWebEngineQuotaRequest quotaRequest);
    void    recentlyAudibleChanged(bool recentlyAudible);
    void    registerProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);
    void    renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode);
    void    scrollPositionChanged(const QPointF &position);
    void    selectionChanged();
    void    titleChanged(const QString &title);
    void    urlChanged(const QUrl &url);
    void    windowCloseRequested();

};

#endif
