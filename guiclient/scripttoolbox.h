/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __SCRIPTTOOLBOX_H__
#define __SCRIPTTOOLBOX_H__

#include <QObject>
#include <QtScript>

#include <parametergroup.h>

#include "parameter.h"
#include "guiclient.h"

#include "addresscluster.h"     // for AddressCluster::SaveFlags

class QWidget;
class QScriptEngine;

/* TODO: remove this enum and use AddressCluster::SaveFlags directly
   for some reason working with AddressCluster::SaveFlags failed but this works.
 */
enum SaveFlags
{ CHECK = AddressCluster::CHECK,
  CHANGEONE = AddressCluster::CHANGEONE,
  CHANGEALL = AddressCluster::CHANGEALL
};

Q_DECLARE_METATYPE(enum SetResponse)
Q_DECLARE_METATYPE(enum GUIClient::WindowSystem);
Q_DECLARE_METATYPE(enum SaveFlags);

class ScriptToolbox : public QObject
{
  Q_OBJECT

  public:
    ScriptToolbox(QScriptEngine * engine);
    virtual ~ScriptToolbox();

    static void setLastWindow(QWidget * lw);

    // expose format.h
    Q_INVOKABLE int     decimalPlaces(QString p)	        { return ::decimalPlaces(p); };
    Q_INVOKABLE QString formatNumber(double value, int decimals){ return ::formatNumber(value, decimals); };
    Q_INVOKABLE QString formatMoney(double val, int curr = -1, int extra = 0)	{ return ::formatMoney(val, curr, extra); };
    Q_INVOKABLE QString formatCost(double val, int curr= -1)	{ return ::formatCost(val, curr); };
    Q_INVOKABLE QString formatExtPrice(double val, int curr=-1)	{ return ::formatExtPrice(val, curr); };
    Q_INVOKABLE QString formatWeight(double val)	        { return ::formatWeight(val); };
    Q_INVOKABLE QString formatQty(double val)	                { return ::formatQty(val); };
    Q_INVOKABLE QString formatQtyPer(double val)	        { return ::formatQtyPer(val); };
    Q_INVOKABLE QString formatSalesPrice(double val, int curr = -1)	{ return ::formatSalesPrice(val, curr); };
    Q_INVOKABLE QString formatPurchPrice(double val, int curr = -1)	{ return ::formatPurchPrice(val, curr); };
    Q_INVOKABLE QString formatUOMRatio(double val)	        { return ::formatUOMRatio(val); };
    Q_INVOKABLE QString formatPercent(double val)	        { return ::formatPercent(val); };
    Q_INVOKABLE QColor  namedColor(QString name)	        { return ::namedColor(name); };
    Q_INVOKABLE QString formatDate(const QDate &pDate)          { return ::formatDate(pDate); };

  public slots:

    XSqlQuery executeQuery(const QString & query);
    XSqlQuery executeQuery(const QString & query, const ParameterList & params);
    XSqlQuery executeDbQuery(const QString & group, const QString & name);
    XSqlQuery executeDbQuery(const QString & group, const QString & name, const ParameterList & params);
    XSqlQuery executeBegin();
    XSqlQuery executeCommit();
    XSqlQuery executeRollback();

    QObject * customVal(const QString & ReqExp);

    QObject * widgetGetLayout(QWidget * w);

    int       menuActionCount(QObject * menu);

    QWidget * createWidget(const QString & className, QWidget * parent = 0, const QString & name = QString());
    QObject * createLayout(const QString & className, QWidget * parent, const QString & name = QString());
    QWidget * loadUi(const QString & screenName, QWidget * parent = 0);

    QWidget * lastWindow() const;
    QWidget * openWindow(const QString pname, QWidget *parent = 0, Qt::WindowModality modality = Qt::NonModal, Qt::WindowFlags flags = 0);
    QWidget * newDisplay(const QString pname, QWidget *parent = 0, Qt::WindowModality modality = Qt::NonModal, Qt::WindowFlags flags = 0);

#if QT_VERSION < 0x050900
    void loadQWebView(QWidget * webView, const QString & url);
#endif

    bool printReport(const QString & name, const ParameterList & params, const QString & pdfFilename = QString::null);
    bool printReport(const QString & name, const ParameterList & params, const bool preview, QWidget *parent = 0);
    bool printReportCopies(const QString & name, const ParameterList & params, int copies);

    bool coreDisconnect(QObject * sender, const QString & signal, QObject * receiver, const QString & method);

    QString fileDialog(QWidget * parent, const QString & caption, const QString & dir, const QString & filter, int fileModeSel, int acceptModeSel);
    void openUrl(const QString & fileUrl);

    void    listProperties(const QScriptValue &obj, const bool showPrototype = true) const;
    QString textStreamRead(const QString & name);
    bool    textStreamWrite(const QString & name, const QString & WriteText);

    int     saveCreditCard(QWidget *parent,
                              int custId,
                              QString ccName,
                              QString ccAddress1,
                              QString ccAddress2,
                              QString ccCity,
                              QString ccState,
                              QString ccZip,
                              QString ccCountry,
                              QString ccNumber,
                              QString ccType,
                              QString ccExpireMonth,
                              QString ccExpireYear,
                              int ccId = 0,
                              bool ccActive = true );
    QObject *getCreditCardProcessor();

    QString storedProcErrorLookup(const QString proc, const int result);

  private:
    QScriptEngine * _engine;
    static QWidget * _lastWindow;
};

#endif // __SCRIPTTOOLBOX_H__
