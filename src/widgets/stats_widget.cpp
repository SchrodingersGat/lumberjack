#include <qtablewidget.h>

#include "stats_widget.hpp"


StatsWidget::StatsWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    setWindowTitle("Statistics");

    initTable();
}


void StatsWidget::initTable()
{
    auto* table = ui.statsTable;

    table->clear();

    QStringList headers;

    headers << tr("Series");
    headers << tr("Min");
    headers << tr("Max");
    headers << tr("Mean");
//    headers << tr("Std Dev");

    table->setColumnCount(headers.length());
    table->setHorizontalHeaderLabels(headers);
}


void StatsWidget::updateStats(const QList<DataSeriesPointer> &seriesList, const QwtInterval &interval)
{
    auto* table = ui.statsTable;

    double tMin = interval.minValue();
    double tMax = interval.maxValue();

    // Remove any extra rows
    while (table->rowCount() > seriesList.length())
    {
        table->removeRow(table->rowCount() - 1);
    }

    // Add any extra rows
    while (table->rowCount() < seriesList.length())
    {
        int row = table->rowCount();
        table->insertRow(row);

        for (int ii = 0; ii < table->columnCount(); ii++)
        {
            table->setItem(row, ii, new QTableWidgetItem(""));
        }
    }

    // TODO: Make this a threaded function, could take a long time to calculate

    // Fill out the data
    for (int idx = 0; idx < seriesList.count(); idx++)
    {
        auto series = seriesList.at(idx);

        if (series.isNull()) continue;

        // TODO: If the series does not have any data points within the interval,
        //       then we should simply ignore it (or display dashes)

        double vMin = series->getMinimumValue(tMin, tMax);
        double vMax = series->getMaximumValue(tMin, tMax);
        double vMean = series->getMeanValue(tMin, tMax);

        table->item(idx, 0)->setText(series->getLabel());
        table->item(idx, 1)->setText(QString::number(vMin));
        table->item(idx, 2)->setText(QString::number(vMax));
        table->item(idx, 3)->setText(QString::number(vMean));
    }

}

