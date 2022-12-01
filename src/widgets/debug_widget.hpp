#ifndef DEBUG_WIDGET_HPP
#define DEBUG_WIDGET_HPP

#include <QWidget>
#include <qtimer.h>

#include "ui_debug_widget.h"

class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    DebugWidget(QWidget *parent = nullptr);

public slots:
    void debugPreferencesChanged();
    void clearDebugMessages();

    void updateDebugMessages();

protected:
    Ui::debugForm ui;

    QTimer* updateTimer = nullptr;

    qint64 latestTimestamp = 0;

    void addDebugMessage(QtMsgType type, QString &msg);
};

#endif // DEBUG_WIDGET_HPP
