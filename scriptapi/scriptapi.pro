include( ../global.pri )
TARGET = xtuplescriptapi
TEMPLATE = lib
CONFIG += qt warn_on staticlib

QT += core network printsupport script sql widgets xml
QT += websockets webchannel serialport

lessThan (QT_MINOR_VERSION, 6) : isEqual(QT_MAJOR_VERSION, 5) {
  QT += webkit webkitwidgets
} else {
  QT += webengine webenginewidgets
}

DBFILE = scriptapi.db
LANGUAGE = C++
INCLUDEPATH += $${XTUPLE_DIR}/common          $${XTUPLE_BLD}/common \
               $${XTUPLE_DIR}/scriptapi       $${XTUPLE_BLD}/scriptapi

INCLUDEPATH = $$unique(INCLUDEPATH)
DEPENDPATH += $${INCLUDEPATH}
DESTDIR = ../lib
MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = tmp

HEADERS += setupscriptapi.h \
    include.h \
    scriptapi_internal.h \
    char.h \
    engineevaluate.h \
    jsconsole.h \
    metasqlhighlighterproto.h \
    orreportproto.h \
    parametereditproto.h \
    parameterlistsetup.h \
    qabstractsocketproto.h \
    qactionproto.h \
    qapplicationproto.h \
    qboxlayoutproto.h \
    qbufferproto.h \
    qbuttongroupproto.h \
    qbytearrayproto.h \
    qcoreapplicationproto.h \
    qcryptographichashproto.h \
    qdatawidgetmapperproto.h    \
    qdialogbuttonboxproto.h \
    qdialogsetup.h \
    qdirproto.h \
    qdnsdomainnamerecordproto.h \
    qdnshostaddressrecordproto.h \
    qdnslookupproto.h \
    qdnsmailexchangerecordproto.h \
    qdnsservicerecordproto.h \
    qdnstextrecordproto.h \
    qdockwidgetproto.h \
    qdomattrproto.h \
    qdomcdatasectionproto.h \
    qdomcharacterdataproto.h \
    qdomcommentproto.h \
    qdomdocumentfragmentproto.h \
    qdomdocumentproto.h \
    qdomdocumenttypeproto.h \
    qdomelementproto.h \
    qdomentityproto.h \
    qdomentityreferenceproto.h \
    qdomimplementationproto.h \
    qdomnamednodemapproto.h \
    qdomnodelistproto.h \
    qdomnodeproto.h \
    qdomnotationproto.h \
    qdomprocessinginstructionproto.h \
    qdomtextproto.h \
    qdoublevalidatorproto.h \
    qeventloopproto.h       \
    qeventproto.h \
    qfileinfoproto.h \
    qfileproto.h \
    qfontproto.h \
    qformlayoutproto.h \
    qgridlayoutproto.h \
    qhostaddressproto.h \
    qhostinfoproto.h \
    qiconproto.h \
    qinputdialogproto.h \
    qintvalidatorproto.h \
    qiodeviceproto.h \
    qitemdelegateproto.h \
    qjsondocumentproto.h \
    qjsonobjectproto.h \
    qjsonvalueproto.h \
    qlayoutitemproto.h \
    qlayoutproto.h \
    qmainwindowproto.h \
    qmenubarproto.h \
    qmenuproto.h \
    qmessageboxsetup.h \
    qmimedatabaseproto.h \
    qmimetypeproto.h \
    qnetworkaccessmanagerproto.h \
    qnetworkinterfaceproto.h \
    qnetworkreplyproto.h \
    qnetworkrequestproto.h \
    qobjectproto.h \
    qprinterproto.h \
    qprocessenvironmentproto.h \
    qprocessproto.h \
    qpushbuttonproto.h \
    qscrollarea.h \
    qserialportinfoproto.h \
    qserialportproto.h \
    qsettings.h \
    qsizepolicyproto.h \
    qspaceritemproto.h \
    qsqldatabaseproto.h \
    qsqldriverproto.h \
    qsqlerrorproto.h \
    qsqlproto.h \
    qsqlqueryproto.h \
    qsqlrecordproto.h \
    qsqltablemodelproto.h \
    qsslcertificateextensionproto.h \
    qsslcertificateproto.h \
    qsslcipherproto.h \
    qsslconfigurationproto.h \
    qsslellipticcurveproto.h \
    qsslerrorproto.h \
    qsslkeyproto.h \
    qsslpresharedkeyauthenticatorproto.h \
    qsslproto.h \
    qsslsocketproto.h \
    qstackedwidgetproto.h \
    qtabwidgetproto.h \
    qtablewidgetproto.h \
    qtablewidgetitemproto.h \
    qtcpserverproto.h \
    qtcpsocketproto.h \
    qtextdocumentproto.h \
    qtexteditproto.h \
    qtimerproto.h \
    qtoolbarproto.h \
    qtoolbuttonproto.h \
    qtreewidgetitemproto.h \
    qtsetup.h \
    qudpsocketproto.h \
    qurlproto.h \
    qurlqueryproto.h \
    quuidproto.h \
    qvalidatorproto.h \
    qwebchannelproto.h \
    qwebsocketcorsauthenticatorproto.h \
    qwebsocketproto.h \
    qwebsocketprotocolproto.h \
    qwebsocketserverproto.h \
    qwidgetproto.h \
    webchanneltransport.h \
    xsqlqueryproto.h \
    xvariantsetup.h \
    xwebsync.h \
    xwebsync_p.h \

lessThan (QT_MINOR_VERSION, 6) : equals(QT_MAJOR_VERSION, 5) {
} else {
  HEADERS += qwebenginepageproto.h \
              qwebengineviewproto.h \
              qwebenginesettingsproto.h

  SOURCES += qwebenginepageproto.cpp \
              qwebengineviewproto.cpp \
              qwebenginesettingsproto.cpp
}

SOURCES += setupscriptapi.cpp \
    include.cpp \
    char.cpp \
    engineevaluate.cpp \
    jsconsole.cpp \
    metasqlhighlighterproto.cpp \
    orreportproto.cpp \
    parametereditproto.cpp \
    parameterlistsetup.cpp \
    qabstractsocketproto.cpp \
    qactionproto.cpp \
    qapplicationproto.cpp \
    qboxlayoutproto.cpp \
    qbufferproto.cpp \
    qbuttongroupproto.cpp \
    qbytearrayproto.cpp \
    qcoreapplicationproto.cpp \
    qcryptographichashproto.cpp \
    qdatawidgetmapperproto.cpp  \
    qdialogbuttonboxproto.cpp \
    qdialogsetup.cpp \
    qdirproto.cpp \
    qdnsdomainnamerecordproto.cpp \
    qdnshostaddressrecordproto.cpp \
    qdnslookupproto.cpp \
    qdnsmailexchangerecordproto.cpp \
    qdnsservicerecordproto.cpp \
    qdnstextrecordproto.cpp \
    qdockwidgetproto.cpp \
    qdomattrproto.cpp \
    qdomcdatasectionproto.cpp \
    qdomcharacterdataproto.cpp \
    qdomcommentproto.cpp \
    qdomdocumentfragmentproto.cpp \
    qdomdocumentproto.cpp \
    qdomdocumenttypeproto.cpp \
    qdomelementproto.cpp \
    qdomentityproto.cpp \
    qdomentityreferenceproto.cpp \
    qdomimplementationproto.cpp \
    qdomnamednodemapproto.cpp \
    qdomnodelistproto.cpp \
    qdomnodeproto.cpp \
    qdomnotationproto.cpp \
    qdomprocessinginstructionproto.cpp \
    qdomtextproto.cpp \
    qdoublevalidatorproto.cpp \
    qeventloopproto.cpp       \
    qeventproto.cpp \
    qfileinfoproto.cpp \
    qfileproto.cpp \
    qfontproto.cpp \
    qformlayoutproto.cpp \
    qgridlayoutproto.cpp \
    qhostaddressproto.cpp \
    qhostinfoproto.cpp \
    qiconproto.cpp \
    qinputdialogproto.cpp \
    qintvalidatorproto.cpp \
    qiodeviceproto.cpp \
    qitemdelegateproto.cpp \
    qjsondocumentproto.cpp \
    qjsonobjectproto.cpp \
    qjsonvalueproto.cpp \
    qlayoutitemproto.cpp \
    qlayoutproto.cpp \
    qmainwindowproto.cpp \
    qmenubarproto.cpp \
    qmenuproto.cpp \
    qmessageboxsetup.cpp \
    qmimedatabaseproto.cpp \
    qmimetypeproto.cpp \
    qnetworkaccessmanagerproto.cpp \
    qnetworkinterfaceproto.cpp \
    qnetworkreplyproto.cpp \
    qnetworkrequestproto.cpp \
    qobjectproto.cpp \
    qprinterproto.cpp \
    qprocessenvironmentproto.cpp \
    qprocessproto.cpp \
    qpushbuttonproto.cpp \
    qscrollarea.cpp \
    qserialportinfoproto.cpp \
    qserialportproto.cpp \
    qsettings.cpp \
    qsizepolicyproto.cpp \
    qspaceritemproto.cpp \
    qsqldatabaseproto.cpp \
    qsqldriverproto.cpp \
    qsqlerrorproto.cpp \
    qsqlproto.cpp \
    qsqlqueryproto.cpp \
    qsqlrecordproto.cpp \
    qsqltablemodelproto.cpp \
    qsslcertificateextensionproto.cpp \
    qsslcertificateproto.cpp \
    qsslcipherproto.cpp \
    qsslconfigurationproto.cpp \
    qsslellipticcurveproto.cpp \
    qsslerrorproto.cpp \
    qsslkeyproto.cpp \
    qsslpresharedkeyauthenticatorproto.cpp \
    qsslproto.cpp \
    qsslsocketproto.cpp \
    qstackedwidgetproto.cpp \
    qtabwidgetproto.cpp \
    qtablewidgetproto.cpp \
    qtablewidgetitemproto.cpp \
    qtcpserverproto.cpp \
    qtcpsocketproto.cpp \
    qtextdocumentproto.cpp \
    qtexteditproto.cpp \
    qtimerproto.cpp \
    qtoolbarproto.cpp \
    qtoolbuttonproto.cpp \
    qtreewidgetitemproto.cpp \
    qtsetup.cpp \
    qudpsocketproto.cpp \
    qurlproto.cpp \
    qurlqueryproto.cpp \
    quuidproto.cpp \
    qvalidatorproto.cpp \
    qwebchannelproto.cpp \
    qwebsocketcorsauthenticatorproto.cpp \
    qwebsocketproto.cpp \
    qwebsocketprotocolproto.cpp \
    qwebsocketserverproto.cpp \
    qwidgetproto.cpp \
    webchanneltransport.cpp \
    xsqlqueryproto.cpp \
    xvariantsetup.cpp \
    xwebsync.cpp
