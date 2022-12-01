#ifndef LUMBERJACK_DEBUG_HPP
#define LUMBERJACK_DEBUG_HPP

#include <qdebug.h>

class LumberjackDebugMessage
{
public:
    LumberjackDebugMessage(QtMsgType type, const QString& msg);

    QtMsgType messageType;
    QString message;
};

void lumberjackDebugHandler(QtMsgType msgType, const QMessageLogContext& context, const QString& message);


QList<LumberjackDebugMessage> getDebugMessages();
void clearDebugMessages();


#endif // LUMBERJACK_DEBUG_HPP
