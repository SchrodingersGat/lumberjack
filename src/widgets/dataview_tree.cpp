#include <QDataStream>
#include <QDrag>
#include <QMimeData>
#include <qmenu.h>
#include <qaction.h>
#include <qheaderview.h>

#include "datatable_widget.hpp"
#include "series_editor_dialog.hpp"
#include "dataview_tree.hpp"
#include "data_source_manager.hpp"


DataViewTree::DataViewTree(QWidget *parent) : QTreeWidget(parent)
{
    setupTree();

    connect(this, &QTreeWidget::itemDoubleClicked, this, &DataViewTree::onItemDoubleClicked);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::customContextMenuRequested, this, &DataViewTree::onContextMenu);

    setAlternatingRowColors(true);
}

DataViewTree::~DataViewTree()
{
    // TODO
}


/*
 * Initialize the tree display
 */
void DataViewTree::setupTree()
{
    clear();

    QStringList labels;

    setColumnCount(2);

    labels.append("");
    labels.append(tr("Label"));

    setHeaderLabels(labels);

    setUniformRowHeights(true);

    // Enable drag and drop
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);

    setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    header()->setSectionResizeMode(0, QHeaderView::Fixed);
    header()->setSectionResizeMode(1, QHeaderView::Stretch);
    header()->setStretchLastSection(true);
    header()->resizeSection(0, 55);
}


/*
 * Callback function to edit a particular DataSeries.
 */
void DataViewTree::editDataSeries(DataSeriesPointer series)
{
    if (series.isNull())
    {
        return;
    }

    SeriesEditorDialog *dlg = new SeriesEditorDialog(series);

    int result = dlg->exec();

    if (result)
    {
        refresh(filterString);
    }

    dlg->deleteLater();

}


/*
 * Callback when a right-click context menu is created for a particular item
 */
void DataViewTree::onContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = itemAt(pos);

    if (!item)
    {
        return;
    }

    auto *parent = item->parent();
    auto *manager = DataSourceManager::getInstance();

    QMenu menu(this);

    if (parent)
    {
        // Right-clicked on a "DataSeries" object

        QString source_label = parent->text(1);
        QString series_label = item->text(1);

        auto source = manager->getSourceByLabel(source_label);

        if (source.isNull())
        {
            return;
        }

        auto series = source->getSeriesByLabel(series_label);

        if (series.isNull())
        {
            return;
        }

        // Export series
        QAction *exportSeries = new QAction(tr("Export Series"), &menu);

        // Edit series
        QAction *editSeries = new QAction(tr("Edit Series"), &menu);

        // View data
        QAction *viewSeriesData = new QAction(tr("View Data"), &menu);

        // Delete series
        QAction *deleteSeries = new QAction(tr("Delete Series"), &menu);

        menu.addAction(exportSeries);
        menu.addSeparator();
        menu.addAction(editSeries);
        menu.addAction(viewSeriesData);
        menu.addSeparator();
        menu.addAction(deleteSeries);

        QAction *action = menu.exec(mapToGlobal(pos));

        if (action == exportSeries)
        {
            QList<DataSeriesPointer> dataSeries;
            dataSeries << series;

            DataSourceManager::getInstance()->exportData(dataSeries);
        }
        else if (action == editSeries)
        {
            editDataSeries(series);
        }
        else if (action == viewSeriesData)
        {
            DataSeriesTableView *table = new DataSeriesTableView(series);
            table->show();
        }
        else if (action == deleteSeries)
        {
            emit onSeriesRemoved(series);
            source->removeSeries(series);
        }
    }
    else
    {
        // Right-clicked on a "DataSource" object
        QString label = item->text(1);

        auto source = manager->getSourceByLabel(label);

        if (source.isNull())
        {
            return;
        }

        // Delete source
        QAction *deleteSource = new QAction(tr("Delete Source"), &menu);

        menu.addAction(deleteSource);

        QAction *action = menu.exec(mapToGlobal(pos));

        if (action == deleteSource)
        {
            // Emit "removed" signal for each data series
            for (auto label : source->getSeriesLabels())
            {
                auto series = source->getSeriesByLabel(label);

                if (series.isNull()) continue;

                emit onSeriesRemoved(series);
            }

            manager->removeSource(source);
        }
    }
}


/*
 * Callback when user double-clicks on an item
 */
void DataViewTree::onItemDoubleClicked(QTreeWidgetItem *item, int col)
{
    Q_UNUSED(col);

    if (!item) return;

    auto *parent = item->parent();

    auto *manager = DataSourceManager::getInstance();

    // Double clicked on a DataSeries
    if (parent)
    {
        QString source_label = parent->text(1);
        QString series_label = item->text(1);

        auto series = manager->findSeries(source_label, series_label);

        editDataSeries(series);

    }
    else
    {
        // TODO - Double clicked on a DataSource
    }
}


/*
 * Refresh (redraw) the tree, based on user filtering.
 */
int DataViewTree::refresh(QString filters)
{
    setUpdatesEnabled(false);

    setupTree();

    // Save the filter text
    filterString = filters;

    auto *manager = DataSourceManager::getInstance();

    int series_count = 0;

    for (int ii = 0; ii < manager->getSourceCount(); ii++)
    {
        auto source = manager->getSourceByIndex(ii);

        if (source.isNull()) continue;

        QStringList labels = source->getSeriesLabels(filters);

        // Add a header item for this "data source"
        QTreeWidgetItem *item = new QTreeWidgetItem();

        QString uid = source->getIdentifier();

        item->setText(1, source->getLabel());

        // Embolden text for "source"
        QFont font = item->font(1);
        font.setBold(true);
        item->setFont(1, font);

        int idx = topLevelItemCount();

        if (labels.isEmpty() && !filters.isEmpty())
        {
            continue;
        }

        insertTopLevelItem(idx, item);

        // Construct a flat list of childItems to add to the tree in a single operation
        QList<QTreeWidgetItem*> childItems;

        for (QString series_label : labels)
        {
            auto series = source->getSeriesByLabel(series_label);

            if (series.isNull()) continue;

            QTreeWidgetItem *child = new QTreeWidgetItem();

            // Series label
            child->setText(1, series->getLabel());
            child->setToolTip(1, source->getLabel() + ":" + series->getLabel() + " (" + QString::number(series->size()) + " samples)");

            QFont font = child->font(1);
            font.setItalic(true);
            child->setFont(1, font);

            child->setBackground(0, series->getColor());

            item->addChild(child);

            series_count++;
        }

        item->addChildren(childItems);
    }

    setUpdatesEnabled(true);

    expandAll();

    return series_count;
}


/**
 * @brief DataViewTree::startDrag is called when a "drag" event is initiated
 * @param supported_actions
 */
void DataViewTree::startDrag(Qt::DropActions supported_actions)
{
    auto items = selectedItems();

    // Extract source / series information for each selected DataSeries item
    QStringList source_labels;
    QStringList series_labels;

    for (auto *item : items)
    {
        if (!item) continue;

        auto *parent = item->parent();

        // Prevent top-level items (data sources) from being dragged
        if (!parent) continue;

        source_labels << parent->text(1);
        series_labels << item->text(1);
    }

    // Nothing draggable was selected (e.g. only data sources were selected)
    if (source_labels.isEmpty())
    {
        return;
    }

    QByteArray source_data;
    QByteArray series_data;

    QDataStream source_stream(&source_data, QIODevice::WriteOnly);
    QDataStream series_stream(&series_data, QIODevice::WriteOnly);

    source_stream << source_labels;
    series_stream << series_labels;

    QDrag *dragger = new QDrag(this);

    auto *mime = new QMimeData();

    mime->setData("source", source_data);
    mime->setData("series", series_data);

    dragger->setMimeData(mime);

    dragger->exec(supported_actions);
}
