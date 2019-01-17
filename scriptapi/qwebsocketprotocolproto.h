/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QWEBSOCKETPROTOCOLPROTO_H__
#define __QWEBSOCKETPROTOCOLPROTO_H__

#include <QtScript>

void setupQWebSocketProtocolProto(QScriptEngine *engine);

#if QT_VERSION >= 0x050000
#include <QtWebSockets/qwebsocketprotocol.h>

Q_DECLARE_METATYPE(enum QWebSocketProtocol::CloseCode)
Q_DECLARE_METATYPE(enum QWebSocketProtocol::Version)
#endif
#endif
