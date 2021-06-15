#include <QDrag>
#include <QMimeData>

#include "series_editor_dialog.hpp"
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

    connect(this, &QTreeWidget::itemDoubleClicked, this, &DataViewTree::onItemDoubleClicked);
}


DataViewTree::~DataViewTree()
{
    // TODO
}


void DataViewTree::onItemDoubleClicked(QTreeWidgetItem *item, int col)
{
    if (!item) return;

    auto *parent = item->parent();

    auto *manager = DataSourceManager::getInstance();

    // Double clicked on a DataSeries
    if (parent)
    {
        QString source_label = parent->text(0);
        QString series_label = item->text(0);

        auto series = manager->findSeries(source_label, series_label);

        if (series.isNull()) return;

        SeriesEditorDialog *dlg = new SeriesEditorDialog(series);

        int result = dlg->exec();

        dlg->deleteLater();
    }
    else
    {
        // TODO - Double clicked on a DataSource
    }
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

            // Series label
            child->setText(0, series->getLabel());

            // Series data
            child->setText(2, QString::number(series->size()));

            item->addChild(child);

            series_count++;
        }
    }

    expandAll();

    return series_count;
}


/**
 * @brief DataViewTree::startDrag is called when a "drag" event is initiated
 * @param supported_actions
 */
void DataViewTree::startDrag(Qt::DropActions supported_actions)
{
    // TODO
    auto items = selectedItems();

    // Only allow dragging for single items
    if (items.count() != 1)
    {
        return;
    }

    auto *item = items.at(0);

    // null check
    if (!item) return;

    auto *parent = item->parent();

    // Prevent top-level items from being dragged
    if (!parent)
    {
        qDebug() << "parent!";
        return;
    }

    // Extract source / series information from the item being dragged
    // TODO : A better way of implementing this maybe?

    QString series_label = item->text(0);
    QString source_label = parent->text(0);

    QDrag *dragger = new QDrag(this);

    auto *mime = new QMimeData();

    mime->setData("source", source_label.toLatin1());
    mime->setData("series", series_label.toLatin1());

    dragger->setMimeData(mime);

    dragger->exec(supported_actions);
}
