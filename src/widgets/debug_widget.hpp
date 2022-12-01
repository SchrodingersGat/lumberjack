#ifndef DEBUG_WIDGET_HPP
#define DEBUG_WIDGET_HPP

#include <QWidget>

#include "ui_debug_widget.h"

class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    DebugWidget(QWidget *parent = nullptr);

protected:
    Ui::debugForm ui;
};

#endif // DEBUG_WIDGET_HPP
