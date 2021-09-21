#ifndef STATS_WIDGET_HPP
#define STATS_WIDGET_HPP

#include <QWidget>

#include "ui_stats_view.h"


class StatsWidget : public QWidget
{
    Q_OBJECT

public:
    StatsWidget(QWidget *parent = nullptr);

protected:
    Ui::stats_form ui;

    void initTable();
};

#endif // STATS_WIDGET_HPP
