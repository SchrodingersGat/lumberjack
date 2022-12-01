#include <qscrollbar.h>

#include "debug_widget.hpp"

#include "lumberjack_debug.hpp"
#include "lumberjack_settings.hpp"


DebugWidget::DebugWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    // Button callbacks
    connect(ui.clearAll, &QPushButton::released, this, &DebugWidget::clearDebugMessages);

    connect(ui.showFatal, &QCheckBox::released, this, &DebugWidget::debugPreferencesChanged);
    connect(ui.showCritical, &QCheckBox::released, this, &DebugWidget::debugPreferencesChanged);
    connect(ui.showWarning, &QCheckBox::released, this, &DebugWidget::debugPreferencesChanged);
    connect(ui.showInfo, &QCheckBox::released, this, &DebugWidget::debugPreferencesChanged);
    connect(ui.showDebug, &QCheckBox::released, this, &DebugWidget::debugPreferencesChanged);

    auto *settings = LumberjackSettings().getInstance();

    ui.showFatal->setChecked(settings->loadBoolean("debug", "showFatal"));
    ui.showCritical->setChecked(settings->loadBoolean("debug", "showCritical"));
    ui.showWarning->setChecked(settings->loadBoolean("debug", "showWarning"));
    ui.showInfo->setChecked(settings->loadBoolean("debug", "showInfo"));
    ui.showDebug->setChecked(settings->loadBoolean("debug", "showDebug"));

    updateDebugMessages();

    // Periodically refresh messages
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &DebugWidget::updateDebugMessages);
    updateTimer->start(250);
}


/*
 * Callback when one of the message type selection checkboxes is activated
 */
void DebugWidget::debugPreferencesChanged()
{
    auto *settings = LumberjackSettings().getInstance();

    settings->saveSetting("debug", "showFatal", ui.showFatal->isChecked());
    settings->saveSetting("debug", "showCritical", ui.showCritical->isChecked());
    settings->saveSetting("debug", "showWarning", ui.showWarning->isChecked());
    settings->saveSetting("debug", "showInfo", ui.showInfo->isChecked());
    settings->saveSetting("debug", "showDebug", ui.showDebug->isChecked());

    updateDebugMessages();
}


void DebugWidget::updateDebugMessages()
{
    auto messages = getLumberjackDebugMessages(latestTimestamp);

    for (auto &msg : messages)
    {
        qint64 t = msg.timestamp;

        latestTimestamp = t;

        // Extract timestamp
        QString text;

        // Milliseconds
        text += "." + QString::number(t % 1000).rightJustified(3, '0');
        t /= 1000;

        // Seconds
        text = ":" + QString::number(t % 60).rightJustified(2, '0') + text;
        t /= 60;

        // Minutes
        text = ":" + QString::number(t % 60).rightJustified(2, '0') + text;
        t /= 60;

        // Hours
        text = QString::number(t) + text;

        text += " - ";
        text += msg.message;
        text += "<br>";

        addDebugMessage(msg.messageType, text);
    }
}


/*
 * Append a single debug message to the display
 */
void DebugWidget::addDebugMessage(QtMsgType type, QString &msg)
{
    QString color = "black";

    switch (type)
    {
    case QtMsgType::QtCriticalMsg:
        color = "red";
        break;
    case QtMsgType::QtFatalMsg:
        color = "orange";
        break;
    case QtMsgType::QtWarningMsg:
        color = "yellow";
        break;
    case QtMsgType::QtInfoMsg:
        color = "blue";
        break;
    case QtMsgType::QtDebugMsg:
        color = "green";
        break;
    default:
        break;
    }

    QString html;

    html += "<font color='" + color + "'>";
    html += msg;
    html += "</font>";

    ui.debugConsole->insertHtml(html);

    QTextCursor cursor = ui.debugConsole->textCursor();
    cursor.movePosition(QTextCursor::End);

    // Scroll to the bottom of the window
    ui.debugConsole->verticalScrollBar()->setValue(ui.debugConsole->verticalScrollBar()->maximum());

    QApplication::processEvents();
}


void DebugWidget::clearDebugMessages()
{
    ui.debugConsole->clear();
    clearLumberjackDebugMessages();
    updateDebugMessages();
}
