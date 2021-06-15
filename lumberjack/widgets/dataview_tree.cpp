#include "data_source.hpp"

#include "dataview_tree.hpp"


DataViewTree::DataViewTree(QWidget *parent) : QTreeWidget(parent)
{
    setColumnCount(3);

    QStringList labels;

    labels.append(tr("Source"));
    labels.append(" ");
    labels.append(tr("Data"));

    setHeaderLabels(labels);

    setUniformRowHeights(true);
    setColumnWidth(1, 25);

    // Enable drag and drop
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);

    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
}


DataViewTree::~DataViewTree()
{
    // TODO
}


int DataViewTree::refresh(QString filters)
{

    clear();

    auto *manager = DataSourceManager::getInstance();

    int series_count = 0;

    for (QString source_label : manager->getSourceLabels())
    {
        auto source = manager->getSourceByLabel(source_label);

        if (source.isNull()) continue;

        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0, source_label);
        item->setText(2, QString::number(source->getSeriesCount()));

        int idx = topLevelItemCount();

        insertTopLevelItem(idx, item);

        for (QString series_label : source->getSeriesLabels(filters))
        {
            auto series = source->getSeriesByLabel(series_label);

            if (series.isNull()) continue;

            QTreeWidgetItem *child = new QTreeWidgetItem(item);

            child->setText(0, series->getLabel());
            child->setText(2, QString::number(series->size()));

            item->addChild(child);

            series_count++;
        }
    }

    expandAll();

    return series_count;
}


void DataViewTree::startDrag(Qt::DropActions supported_actions)
{
    // TODO
}
