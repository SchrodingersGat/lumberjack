#include <QMimeData>

#include "filters_widget.hpp"
#include "data_source_manager.hpp"


FiltersWidget::FiltersWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    setAcceptDrops(true);

    connect(ui.applyFilterButton, &QPushButton::released, this, &FiltersWidget::applyFilter);
    connect(ui.clearItemsButton, &QPushButton::released, this, &FiltersWidget::clearItems);

    refresh();
}


void FiltersWidget::refresh()
{
    bool hasItems = !seriesList.isEmpty();

    ui.applyFilterButton->setEnabled(hasItems);
    ui.clearItemsButton->setEnabled(hasItems);
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
