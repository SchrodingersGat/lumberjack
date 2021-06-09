#include "dataview_widget.hpp"


DataviewWidget::DataviewWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    auto *manager = DataSourceManager::getInstance();

    // Trigger update whenever the data sources are updated!
    connect(manager, &DataSourceManager::sourcesChanged, this, &DataviewWidget::refresh);

    refresh();
}


void DataviewWidget::refresh()
{
    auto *manager = DataSourceManager::getInstance();

    for (auto lbl : manager->getSourceLabels())
    {
        qDebug() << "source:" << lbl;
    }
}
