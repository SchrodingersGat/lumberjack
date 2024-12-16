#ifndef STATS_WIDGET_HPP
#define STATS_WIDGET_HPP

#include <QWidget>

#include <qwt_interval.h>

#include "data_series.hpp"

#include "ui_stats_view.h"


class StatsWidget : public QWidget
{
    Q_OBJECT

public:
    StatsWidget(QWidget *parent = nullptr);

public slots:
    void updateStats(const QList<DataSeriesPointer> &series, const QwtInterval &interval);

protected:
    Ui::stats_form ui;

    void initTable();
};

#endif // STATS_WIDGET_HPP
