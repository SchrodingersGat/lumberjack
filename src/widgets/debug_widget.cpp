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
    auto messages = getLumberjackDebugMessages();

    ui.debug->clear();

    for (auto &msg : messages)
    {
        ui.debug->append(msg.message);
    }
}


void DebugWidget::clearDebugMessages()
{
    clearLumberjackDebugMessages();
}
