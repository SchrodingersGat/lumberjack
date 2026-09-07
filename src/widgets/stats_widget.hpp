#ifndef STATS_WIDGET_HPP
#define STATS_WIDGET_HPP

#include <QWidget>
#include <QStandardItemModel>

#include <qwt_interval.h>

#include "data_series.hpp"
#include "stats_filter_proxy_model.hpp"

#include "ui_stats_view.h"


class StatsWidget : public QWidget
{
    Q_OBJECT

public:
    enum Column
    {
        ColumnSeries = 0,
        ColumnMin,
        ColumnMax,
        ColumnMean,
        ColumnCount,
    };

    enum RangeMode
    {
        RangeVisible = 0,
        RangeWholeTrace,
    };

    StatsWidget(QWidget *parent = nullptr);

public slots:
    void updateStats(const QList<DataSeriesPointer> &series, const QwtInterval &interval);

protected slots:
    void onNameFilterChanged(const QString &text);
    void onValueFilterChanged();
    void onClearFilters();
    void onRangeModeChanged();

protected:
    Ui::stats_form ui;

    QStandardItemModel *model;
    StatsFilterProxyModel *proxyModel;

    QList<DataSeriesPointer> lastSeriesList;
    QwtInterval lastViewInterval;

    void initTable();
    void recomputeStats();
    void setColumnValue(int row, int column, double value, bool hasData);
};

#endif // STATS_WIDGET_HPP
