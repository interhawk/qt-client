/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "setupscriptapi.h"

#include <QtGlobal>
#include <QDebug>
#include <QMessageBox>

#include "applock.h"
#include "metrics.h"
#include "xtsettings.h"
#include "char.h"
#include "engineevaluate.h"
#include "exporthelper.h"
#include "format.h"
#include "include.h"
#include "jsconsole.h"
#include "metasqlhighlighterproto.h"
#include "orreportproto.h"
#include "parametereditproto.h"
#include "parameterlistsetup.h"
#include "qabstractsocketproto.h"
#include "qactionproto.h"
#include "qapplicationproto.h"
#include "qboxlayoutproto.h"
#include "qbufferproto.h"
#include "qbuttongroupproto.h"
#include "qbytearrayproto.h"
#include "qcoreapplicationproto.h"
#include "qcryptographichashproto.h"
#include "qdatawidgetmapperproto.h"
#include "qdialogbuttonboxproto.h"
#include "qdialogsetup.h"
#include "qdirproto.h"
#include "qdnsdomainnamerecordproto.h"
#include "qdnshostaddressrecordproto.h"
#include "qdnslookupproto.h"
#include "qdnsmailexchangerecordproto.h"
#include "qdnsservicerecordproto.h"
#include "qdnstextrecordproto.h"
#include "qdockwidgetproto.h"
#include "qdomattrproto.h"
#include "qdomcdatasectionproto.h"
#include "qdomcharacterdataproto.h"
#include "qdomcommentproto.h"
#include "qdomdocumentfragmentproto.h"
#include "qdomdocumentproto.h"
#include "qdomdocumenttypeproto.h"
#include "qdomelementproto.h"
#include "qdomentityproto.h"
#include "qdomentityreferenceproto.h"
#include "qdomimplementationproto.h"
#include "qdomnamednodemapproto.h"
#include "qdomnodelistproto.h"
#include "qdomnodeproto.h"
#include "qdomnotationproto.h"
#include "qdomprocessinginstructionproto.h"
#include "qdomtextproto.h"
#include "qdoublevalidatorproto.h"
#include "qeventloopproto.h"
#include "qeventproto.h"
#include "qfileinfoproto.h"
#include "qfileproto.h"
#include "qfontproto.h"
#include "qformlayoutproto.h"
#include "qgridlayoutproto.h"
#include "qhostaddressproto.h"
#include "qhostinfoproto.h"
#include "qiconproto.h"
#include "qinputdialogproto.h"
#include "qintvalidatorproto.h"
#include "qiodeviceproto.h"
#include "qitemdelegateproto.h"
#include "qjsondocumentproto.h"
#include "qjsonobjectproto.h"
#include "qjsonvalueproto.h"
#include "qlayoutitemproto.h"
#include "qlayoutproto.h"
#include "qmainwindowproto.h"
#include "qmenubarproto.h"
#include "qmenuproto.h"
#include "qmessageboxsetup.h"
#include "qmimedatabaseproto.h"
#include "qmimetypeproto.h"
#include "qnetworkaccessmanagerproto.h"
#include "qnetworkinterfaceproto.h"
#include "qnetworkreplyproto.h"
#include "qnetworkrequestproto.h"
#include "qobjectproto.h"
#include "qprinterproto.h"
#include "qprocessenvironmentproto.h"
#include "qprocessproto.h"
#include "qpushbuttonproto.h"
#include "qscrollarea.h"
#include "qserialportinfoproto.h"
#include "qserialportproto.h"
#include "qsettings.h"
#include "qsizepolicyproto.h"
#include "qspaceritemproto.h"
#include "qsqldatabaseproto.h"
#include "qsqldriverproto.h"
#include "qsqlerrorproto.h"
#include "qsqlproto.h"
#include "qsqlqueryproto.h"
#include "qsqlrecordproto.h"
#include "qsqltablemodelproto.h"
#include "qsslcertificateextensionproto.h"
#include "qsslcertificateproto.h"
#include "qsslcipherproto.h"
#include "qsslconfigurationproto.h"
#include "qsslellipticcurveproto.h"
#include "qsslerrorproto.h"
#include "qsslkeyproto.h"
#include "qsslpresharedkeyauthenticatorproto.h"
#include "qsslproto.h"
#include "qsslsocketproto.h"
#include "qstackedwidgetproto.h"
#include "qtabwidgetproto.h"
#include "qtablewidgetproto.h"
#include "qtablewidgetitemproto.h"
#include "qtcpserverproto.h"
#include "qtcpsocketproto.h"
#include "qtextdocumentproto.h"
#include "qtexteditproto.h"
#include "qtimerproto.h"
#include "qtoolbarproto.h"
#include "qtoolbuttonproto.h"
#include "qtreewidgetitemproto.h"
#include "qtsetup.h"
#include "qudpsocketproto.h"
#include "qurlproto.h"
#include "qurlqueryproto.h"
#include "quuidproto.h"
#include "qvalidatorproto.h"
#include "qwebchannelproto.h"
#include "qwebsocketcorsauthenticatorproto.h"
#include "qwebsocketproto.h"
#include "qwebsocketprotocolproto.h"
#include "qwebsocketserverproto.h"
#include "qwidgetproto.h"
#include "taxIntegration.h"
#include "webchanneltransport.h"
#include "xsqlqueryproto.h"
#include "xvariantsetup.h"
#include "xwebsync.h"

#if QT_VERSION >= 0x050900
  #include "qwebenginepageproto.h"
  #include "qwebengineviewproto.h"
  #include "qwebenginesettingsproto.h"
#endif

static Preferences *prefs = 0;
/*! \defgroup scriptapi The xTuple ERP Scripting API

  The xTuple ERP Scripting API defines the interface between extension %scripts
  and the xTuple ERP C++ core.

 */

void setupScriptApi(QScriptEngine *engine, Preferences *pPreferences)
{
  engine->installTranslatorFunctions();

  if (pPreferences && ! prefs)
    prefs = pPreferences;

  setupAppLockProto(engine);
  setupXtSettings(engine);
  setupEngineEvaluate(engine);
  setupExportHelper(engine);
  setupInclude(engine);
  setupJSConsole(engine);
  setupMetaSQLHighlighterProto(engine);
  setupOrReportProto(engine);
  setupParameterEditProto(engine);
  setupParameterList(engine);
  setupQAbstractSocketProto(engine);
  setupQActionProto(engine);
  setupQApplicationProto(engine);
  setupQBoxLayoutProto(engine);
  setupQBufferProto(engine);
  setupQButtonGroupProto(engine);
  setupQByteArrayProto(engine);
  setupQCoreApplicationProto(engine);
  setupQCryptographicHashProto(engine);
  setupQDataWidgetMapperProto(engine);
  setupQDialog(engine);
  setupQDialogButtonBoxProto(engine);
  setupQDirProto(engine);
  setupQDnsDomainNameRecordProto(engine);
  setupQDnsHostAddressRecordProto(engine);
  setupQDnsLookupProto(engine);
  setupQDnsMailExchangeRecordProto(engine);
  setupQDnsServiceRecordProto(engine);
  setupQDnsTextRecordProto(engine);
  setupQDockWidgetProto(engine);
  setupQDomAttrProto(engine);
  setupQDomCDATASectionProto(engine);
  setupQDomCharacterDataProto(engine);
  setupQDomCommentProto(engine);
  setupQDomDocumentFragmentProto(engine);
  setupQDomDocumentProto(engine);
  setupQDomDocumentTypeProto(engine);
  setupQDomElementProto(engine);
  setupQDomEntityProto(engine);
  setupQDomEntityReferenceProto(engine);
  setupQDomImplementationProto(engine);
  setupQDomNamedNodeMapProto(engine);
  setupQDomNodeListProto(engine);
  setupQDomNodeProto(engine);
  setupQDomNotationProto(engine);
  setupQDomProcessingInstructionProto(engine);
  setupQDomTextProto(engine);
  setupQDoubleValidatorProto(engine);
  setupQEventLoopProto(engine);
  setupQEventProto(engine);
  setupQFileInfoProto(engine);
  setupQFileProto(engine);
  setupQFontProto(engine);
  setupQFormLayoutProto(engine);
  setupQGridLayoutProto(engine);
  setupQHostAddressProto(engine);
  setupQHostInfoProto(engine);
  setupQIODeviceProto(engine);
  setupQIconProto(engine);
  setupQInputDialogProto(engine);
  setupQIntValidatorProto(engine);
  setupQItemDelegateProto(engine);
  setupQJsonDocumentProto(engine);
  setupQJsonObjectProto(engine);
  setupQJsonValueProto(engine);
  setupQLayoutItemProto(engine);
  setupQLayoutProto(engine);
  setupQMainWindowProto(engine);
  setupQMenuBarProto(engine);
  setupQMenuProto(engine);
  setupQMessageBox(engine);
  setupQMimeDatabaseProto(engine);
  setupQMimeTypeProto(engine);
  setupQNetworkAccessManagerProto(engine);
  setupQNetworkInterfaceProto(engine);
  setupQNetworkReplyProto(engine);
  setupQNetworkRequestProto(engine);
  setupQObjectProto(engine);
  setupQPrinterProto(engine);
  setupQProcessEnvironmentProto(engine);
  setupQProcessProto(engine);
  setupQPushButtonProto(engine);
  setupQScrollAreaProto(engine);
  setupQSerialPortInfoProto(engine);
  setupQSerialPortProto(engine);
  setupQSettingsProto(engine);
  setupQSizePolicy(engine);
  setupQSpacerItem(engine);
  setupQSqlDatabaseProto(engine);
  setupQSqlDriverProto(engine);
  setupQSqlErrorProto(engine);
  setupQSqlProto(engine);
  setupQSqlQueryProto(engine);
  setupQSqlRecordProto(engine);
  setupQSqlTableModelProto(engine);
  setupQSslCertificateExtensionProto(engine);
  setupQSslCertificateProto(engine);
  setupQSslCipherProto(engine);
  setupQSslConfigurationProto(engine);
  setupQSslEllipticCurveProto(engine);
  setupQSslErrorProto(engine);
  setupQSslKeyProto(engine);
  setupQSslPreSharedKeyAuthenticatorProto(engine);
  setupQSslProto(engine);
  setupQSslSocketProto(engine);
  setupQStackedWidgetProto(engine);
  setupQTabWidgetProto(engine);
  setupQTableWidgetProto(engine);
  setupQTableWidgetItemProto(engine);
  setupTaxIntegration(engine);
  setupQTcpServerProto(engine);
  setupQTcpSocketProto(engine);
  setupQTextDocumentProto(engine);
  setupQTextEditProto(engine);
  setupQTimerProto(engine);
  setupQToolBarProto(engine);
  setupQToolButtonProto(engine);
  setupQTreeWidgetItemProto(engine);
  setupQUdpSocketProto(engine);
  setupQUrlProto(engine);
  setupQUrlQueryProto(engine);
  setupQUuidProto(engine);
  setupQValidatorProto(engine);
  setupQWebChannelProto(engine);
  setupQWebSocketCorsAuthenticatorProto(engine);
  setupQWebSocketProto(engine);
  setupQWebSocketProtocolProto(engine);
  setupQWebSocketServerProto(engine);
  setupQWidgetProto(engine);
  setupQt(engine);
  setupWebChannelTransport(engine);
  setupXSqlQueryProto(engine);
  setupXVariant(engine);
  setupXWebSync(engine);
  setupchar(engine);
  setupFormat(engine);

  #if QT_VERSION > 0x050900
    setupQWebEnginePageProto(engine);
    setupQWebEngineViewProto(engine);
    setupQWebEngineSettingsProto(engine);
  #endif
}

void scriptDeprecated(QString msg)
{
  if (! prefs)
    return;

  if (prefs->value("DeprecationLevel") == "debug")
    qDebug() << msg;
  else if (prefs->value("DeprecationLevel") == "info")
    qInfo() << msg;
  else if (prefs->value("DeprecationLevel") == "warning")
    qWarning() << msg;
  else if (prefs->value("DeprecationLevel") == "critical")
    QMessageBox::critical(0, QObject::tr("Deprecated Script Call"), msg);
  else if (prefs->value("DeprecationLevel") == "fatal")
  {
    QMessageBox::critical(0, QObject::tr("Deprecated Script Call"),
                          QObject::tr("Fatal error:<p>%1").arg(msg));
    QApplication::exit(5);
    exit(5);
  }
}
