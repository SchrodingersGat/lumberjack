#include <QDrag>
#include <QMimeData>
#include <qmenu.h>
#include <qaction.h>

#include "datatable_widget.hpp"
#include "series_editor_dialog.hpp"
#include "data_source.hpp"
#include "dataview_tree.hpp"


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


void DataViewTree::setupTree()
{
    static bool configured = false;

    clear();

//    if (configured) return;

    configured = true;

    QStringList labels;

    setColumnCount(3);

    labels.append(tr("Label"));
    labels.append(tr("Data"));
    labels.append("");

    setHeaderLabels(labels);

    setUniformRowHeights(true);

    // Enable drag and drop
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);

    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
}


void DataViewTree::editDataSeries(QSharedPointer<DataSeries> series)
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

        QString source_label = parent->text(0);
        QString series_label = item->text(0);

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

        // Edit series
        QAction *editSeries = new QAction(tr("Edit Series"), &menu);

        // View data
        QAction *viewSeriesData = new QAction(tr("View Data"), &menu);

        // Delete series
        QAction *deleteSeries = new QAction(tr("Delete Series"), &menu);

        menu.addAction(editSeries);
        menu.addAction(viewSeriesData);
        menu.addSeparator();
        menu.addAction(deleteSeries);

        QAction *action = menu.exec(mapToGlobal(pos));

        if (action == editSeries)
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
            source->removeSeries(series);
        }
    }
    else
    {
        // Right-clicked on a "DataSource" object
        QString label = item->text(0);

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
            manager->removeSource(source);
        }
    }
}


void DataViewTree::onItemDoubleClicked(QTreeWidgetItem *item, int col)
{
    Q_UNUSED(col);

    if (!item) return;

    auto *parent = item->parent();

    auto *manager = DataSourceManager::getInstance();

    // Double clicked on a DataSeries
    if (parent)
    {
        QString source_label = parent->text(0);
        QString series_label = item->text(0);

        auto series = manager->findSeries(source_label, series_label);

        editDataSeries(series);

    }
    else
    {
        // TODO - Double clicked on a DataSource
    }
}


int DataViewTree::refresh(QString filters)
{
    setUpdatesEnabled(false);

    setupTree();

    // Save the filter text
    filterString = filters;

    auto *manager = DataSourceManager::getInstance();

    int series_count = 0;

    for (QString source_label : manager->getSourceLabels())
    {
        auto source = manager->getSourceByLabel(source_label);

        QStringList labels = source->getSeriesLabels(filters);

        if (source.isNull()) continue;

        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0, source_label);
        item->setText(1, QString::number(labels.count()) + " series");

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
            child->setText(0, series->getLabel());
            child->setToolTip(0, series->getLabel() + " (" + QString::number(series->size()) + " data points)");

            child->setText(1, QString::number(series->size()));
            child->setBackground(2, series->getColor());

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
