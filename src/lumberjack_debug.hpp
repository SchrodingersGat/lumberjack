#ifndef LUMBERJACK_DEBUG_HPP
#define LUMBERJACK_DEBUG_HPP

#include <qdebug.h>

class LumberjackDebugMessage
{
public:
    LumberjackDebugMessage(qint64 t, QtMsgType type, const QString& msg);

    qint64 timestamp = 0;

    QtMsgType messageType;
    QString message;
};

void registerLumberjackDebugHandler();
QList<LumberjackDebugMessage> getLumberjackDebugMessages(qint64 tSince);
void clearLumberjackDebugMessages();


#endif // LUMBERJACK_DEBUG_HPP
