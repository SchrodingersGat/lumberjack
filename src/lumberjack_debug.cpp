#include <qelapsedtimer.h>

#include "lumberjack_debug.hpp"
#include "lumberjack_settings.hpp"

#include <stdio.h>
#include <stddef.h>
#include <iostream>

void lumberjackDebugHandler(QtMsgType msgType, const QMessageLogContext& context, const QString& message);


QList<LumberjackDebugMessage> messageBuffer;
QElapsedTimer debugTimer;


LumberjackDebugMessage::LumberjackDebugMessage(qint64 t, QtMsgType type, const QString& msg) :
    timestamp(t),
    messageType(type),
    message(msg)
{

}


void lumberjackDebugHandler(QtMsgType msgType, const QMessageLogContext& context, const QString& message)
{
    Q_UNUSED(context)

    // Construct and buffer a new debug message
    messageBuffer.append(LumberjackDebugMessage(debugTimer.elapsed(), msgType, message));
}


void registerLumberjackDebugHandler()
{
    debugTimer.restart();
    qInstallMessageHandler(lumberjackDebugHandler);
}


/*
 * Return a list of debug messages according to the user specified filter
 */
QList<LumberjackDebugMessage> getLumberjackDebugMessages(qint64 tSince)
{
    auto *settings = LumberjackSettings().getInstance();

    uint32_t mask = 0x00;

    // Construct the debug message mask
    if (settings->loadBoolean("debug", "showInfo")) mask |= (1 << QtMsgType::QtInfoMsg);
    if (settings->loadBoolean("debug", "showDebug")) mask |= (1 << QtMsgType::QtDebugMsg);
    if (settings->loadBoolean("debug", "showWarning")) mask |= (1 << QtMsgType::QtWarningMsg);
    if (settings->loadBoolean("debug", "showCritical")) mask |= (1 << QtMsgType::QtCriticalMsg);
    if (settings->loadBoolean("debug", "showFatal")) mask |= (1 << QtMsgType::QtFatalMsg);

    QList<LumberjackDebugMessage> messages;

    for (const auto &msg : messageBuffer)
    {
        // Ignore messages prior to a given timestamp
        if (msg.timestamp <= tSince) continue;

        uint32_t flag = 1 << msg.messageType;
        if (mask & flag) messages.append(msg);
    }

    return messages;
}


void clearLumberjackDebugMessages()
{
    messageBuffer.clear();
}
