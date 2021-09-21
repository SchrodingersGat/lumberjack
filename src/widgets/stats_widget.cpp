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
    headers << tr("Std Dev");

    table->setColumnCount(headers.length());
    table->setHorizontalHeaderLabels(headers);
}



