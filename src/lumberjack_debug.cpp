#include "lumberjack_debug.hpp"
#include "lumberjack_settings.hpp"

#include <stdio.h>
#include <stddef.h>
#include <iostream>

QList<LumberjackDebugMessage> messageBuffer;


LumberjackDebugMessage::LumberjackDebugMessage(QtMsgType type, const QString& msg) :
    messageType(type),
    message(msg)
{

}


void lumberjackDebugHandler(QtMsgType msgType, const QMessageLogContext& context, const QString& message)
{
    Q_UNUSED(context)

    // Construct and buffer a new debug message
    messageBuffer.append(LumberjackDebugMessage(msgType, message));
}


/*
 * Return a list of debug messages according to the user specified filter
 */
QList<LumberjackDebugMessage> getDebugMessages()
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

    for (auto &msg : messageBuffer)
    {
        uint32_t flag = 1 << msg.messageType;

        if (mask & flag) messages.append(msg);
    }

    return messages;
}


void clearDebugMessages()
{
    messageBuffer.clear();
}
