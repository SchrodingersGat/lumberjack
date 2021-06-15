#include "dataview_widget.hpp"


DataviewWidget::DataviewWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    auto *manager = DataSourceManager::getInstance();

    // Trigger update whenever the data sources are updated!
    connect(manager, &DataSourceManager::sourcesChanged, this, &DataviewWidget::refresh);

    // Update whenever filter text is changed
    connect(ui.filterText, &QLineEdit::textEdited, this, &DataviewWidget::filterTextUpdated);

    connect(ui.clearButton, &QPushButton::released, this, &DataviewWidget::clearFilter);

    initTree();

    refresh();
}


/**
 * @brief DataviewWidget::clearFilter callback when the "clear filter" button is pressed
 */
void DataviewWidget::clearFilter()
{
    ui.filterText->clear();
}


void DataviewWidget::filterTextUpdated(QString text)
{
    Q_UNUSED(text);

    refresh();
}


void DataviewWidget::initTree()
{
    auto *tree = ui.treeWidget;

    tree->setColumnCount(3);

    QStringList labels;

    labels.append("Source");
    labels.append(" ");
    labels.append("Data");

    tree->setHeaderLabels(labels);

    tree->setUniformRowHeights(true);
}


void DataviewWidget::refresh()
{
    auto *manager = DataSourceManager::getInstance();

    auto *tree = ui.treeWidget;

    tree->clear();

    QString filter_text = ui.filterText->text();

    for (QString source_label : manager->getSourceLabels())
    {
        auto source = manager->getSourceByLabel(source_label);

        if (source.isNull()) continue;

        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0, source_label);
        item->setText(2, QString::number(source->getSeriesCount()));

        int idx = tree->topLevelItemCount();

        tree->insertTopLevelItem(idx, item);

        for (QString series_label : source->getSeriesLabels(filter_text))
        {
            auto series = source->getSeriesByLabel(series_label);

            if (series.isNull()) continue;

            QTreeWidgetItem *child = new QTreeWidgetItem(item);

            child->setText(0, series->getLabel());
            child->setText(2, QString::number(series->size()));

            item->addChild(child);
        }
    }

    tree->expandAll();
}
