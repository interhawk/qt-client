/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2019 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include <QDebug>
#include <QMetaMethod>

/** @file xtuplecommon.h
 @addtogroup globals Global Variables and Functions
 @{
 */
/** @def DEBUG
  Many source files write tracing information to Qt's debug stream if the
  @c DEBUG macro evaluates to true.
*/
/** @def ENTERED
  Place @c ENTERED after the opening bracket of methods in files that include
  @c guiclient.h or @c widgets.h and define the @c DEBUG macro. @ENTERED
  writes information about the method being called. If the
  method was invoked by a signal, information about the signal is included.

  @c ENTERED generates a simple C++ statement and does not terminate that
  statement, so a trailing semicolon ( @; ) is required. It uses @c qDebug()
  to generate the output, so you can add to the output if you wish with the
  @ << operator.

  @code
  void MyClass::method1()
  {
    ENTERED;
    // do stuff
  }

  void MyClass::method2(Type1 arg1, Type2 arg2)
  {
    ENTERED << "with" << arg1 << arg2;
    // do different stuff
  }
  @endcode

*/
/** @} */

#define ENTERED                                                              \
  if (DEBUG)                                                                 \
      qDebug() << (! this->objectName().isEmpty() ? this->objectName()       \
                                                 : __FILE__)                 \
               << "::" << __func__ << "() entered"                           \
               << (sender() ? QString::asprintf("signaled by %s::%s",       \
                                sender()->objectName().toLatin1().data(),    \
                                sender()->metaObject()                       \
                                        ->method(senderSignalIndex())        \
                                        .methodSignature().data())           \
                            : QString())
