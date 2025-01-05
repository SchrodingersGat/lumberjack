#include <QMimeData>

#include "filters_widget.hpp"

#include "plugin_registry.hpp"
#include "lumberjack_settings.hpp"
#include "data_source_manager.hpp"


FiltersWidget::FiltersWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    setAcceptDrops(true);

    refresh();

    connect(ui.applyFilterButton, &QPushButton::released, this, &FiltersWidget::applyFilter);
    connect(ui.clearItemsButton, &QPushButton::released, this, &FiltersWidget::clearItems);
}


void FiltersWidget::loadPlugins()
{
    auto registry = PluginRegistry::getInstance();
    auto settings = LumberjackSettings::getInstance();

    ui.filterSelect->clear();

    QString selectedPlugin = settings->loadString("filters", "selectedFilter");

    for (auto plugin : registry->FilterPlugins())
    {
        ui.filterSelect->addItem(plugin->pluginName());
    }


    for (int idx = 0; idx < ui.filterSelect->count(); idx++)
    {
        if (ui.filterSelect->itemText(idx) == selectedPlugin)
        {
            ui.filterSelect->setCurrentIndex(idx);
            break;
        }
    }

    refresh();
}


void FiltersWidget::refresh()
{
    bool hasItems = !seriesList.isEmpty();

    ui.applyFilterButton->setEnabled(hasItems);
    ui.clearItemsButton->setEnabled(hasItems);

    updateSeriesList();
}


void FiltersWidget::updateSeriesList(void)
{
    auto *list = ui.seriesList;

    list->clear();

    list->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for (auto series : seriesList)
    {
        if (series.isNull()) continue;

        QListWidgetItem *item = new QListWidgetItem(series->getLabel(), list);
        list->addItem(item);
    }
}


void FiltersWidget::clearItems()
{
    seriesList.clear();
    refresh();
    // TODO - other stuff too?
}


void FiltersWidget::applyFilter()
{
    // TODO: Apply selected filter
    refresh();
}


void FiltersWidget::addSeries(DataSeriesPointer series)
{
    seriesList.append(series);
    refresh();
}


/**
 * @brief FiltersWidget::dragEnterEvent is called when a drag event enters the widget
 * @param event
 */
void FiltersWidget::dragEnterEvent(QDragEnterEvent *event)
{
    auto *mime = event->mimeData();

    // DataSeries is being dragged onto this PlotWidget
    if (mime->hasFormat("source") && mime->hasFormat("series"))
    {
        event->acceptProposedAction();
        return;
    }
}


/**
 * @brief FiltersWidget::dragMoveEvent is called when an accepted drag event moves across the widget
 * @param event
 */
void FiltersWidget::dragMoveEvent(QDragMoveEvent *event)
{
    Q_UNUSED(event)
}


/**
 * @brief FiltersWidget::dropEvent is called when an accepted drag event is dropped on the widget
 * @param event
 */
void FiltersWidget::dropEvent(QDropEvent *event)
{
    auto *mime = event->mimeData();
    auto *manager = DataSourceManager::getInstance();

    if (!mime || !manager)
    {
        return;
    }

    // // DataSeries is dropped onto this PlotWidget
    if (mime->hasFormat("source") && mime->hasFormat("series"))
    {
        QString source_lbl = mime->data("source");
        QString series_lbl = mime->data("series");

        auto series = manager->findSeries(source_lbl, series_lbl);

        if (series.isNull())
        {
            qCritical() << "Could not find series matching" << source_lbl << ":" << series_lbl;
            return;
        }

        event->accept();
        addSeries(series);
    }
}
