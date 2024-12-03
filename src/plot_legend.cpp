#include <QPen>


#include <qevent.h>
#include <qdebug.h>
#include <qwt_plot.h>
#include <qwt_text.h>
#include <qwt_legend_data.h>
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>

#include "plot_legend.hpp"
#include "plot_widget.hpp"


PlotLegend::PlotLegend(PlotWidget *plot) : plot(plot)
{
    // Initial rendering options
    setRenderHint(QwtPlotItem::RenderAntialiased);
    setLineColor(lineColor);
    setFillColor(fillColor);
    setFontSize(fontSize);

    setMaxColumns(1);
    setBackgroundMode(QwtPlotLegendItem::BackgroundMode::LegendBackground);
    setBorderRadius(8);
    setMargin(6);
    setSpacing(4);
    setItemMargin(3);
}


void PlotLegend::setLineColor(const QColor &color)
{
    lineColor = color;

    setTextPen(color);
    setBorderPen(color);
}


void PlotLegend::setFillColor(const QColor &color)
{
    QColor c = color;
    c.setAlpha(150);

    fillColor = c;

    setBackgroundBrush(c);
}


void PlotLegend::setFontSize(int size)
{
    QFont f = font();
    f.setPointSize(size);
    setFont(f);

    fontSize = size;
}


void PlotLegend::setAxisAlignment(QwtAxisId yAxis)
{
    int alignment = 0;

    // TODO: Allow vertical adjustment
    alignment = Qt::AlignTop;

    switch (yAxis)
    {
    case QwtPlot::yRight:
        alignment |= Qt::AlignRight;
        break;
    case QwtPlot::yLeft:
    default:
        alignment |= Qt::AlignLeft;
        break;
    }

    setAlignmentInCanvas(Qt::Alignment(alignment));
    setAxes(QwtPlot::xBottom, yAxis);
}


/**
 * @brief PlotLegend::updateLegend - Called when an item is updated in the legend
 */
void PlotLegend::updateLegend(const QwtPlotItem *item, const QList<QwtLegendData> &data)
{
    const QwtPlotCurve *curve = dynamic_cast<const QwtPlotCurve*>(item);

    if (curve && curve != nullptr)
    {
        if (curve->yAxis() != yAxis())
        {
            // Check if the item already exists in this legend
            bool foundMatch = false;
            for (auto *plotItem : plotItems())
            {
                if (plotItem == item)
                {
                    foundMatch = true;
                }
            }

            // Prevent item addition if the yAxisId values do not match
            if (!foundMatch)
            {
                return;
            }
        }
    }

    QwtPlotLegendItem::updateLegend(item, data);
}


/**
 * @brief PlotLegend::handleMousePressEvent - Process a mouse event within the legend area
 * @param event
 * @return
 */
bool PlotLegend::handleMousePressEvent(const QMouseEvent *event)
{
    if (!plot) return false;
    if (!event) return false;

    QRect rect = geometry(plot->canvas()->rect());

    // Ignore if the legend is not displayed
    if (rect.width() < 0 || rect.height() < 0) return false;

    int x = event->x();
    int y = event->y();

    // Return early if click is outside legend bounds
    if (x < rect.left() || x > rect.right()) return false;
    if (y < rect.top() || y > rect.bottom()) return false;

    const QwtPlotItem *clickedItem = nullptr;

    for (const QwtPlotItem *item : plotItems())
    {
        if (!item) continue;

        QList<QRect> geometries = legendGeometries(item);

        if (geometries.length() == 0) continue;

        QRect itemRect = geometries.at(0);

        for (int idx = 1; idx < geometries.count(); idx++)
        {
            itemRect |= geometries.at(idx);
        }

        if (x < itemRect.left() || x > itemRect.right()) continue;
        if (y < itemRect.top() || y > itemRect.bottom()) continue;

        clickedItem = dynamic_cast<const QwtPlotItem*>(item);
        break;
    }

    bool result = true;

    // A valid item has been detected - handle the event
    if (clickedItem)
    {
        switch (event->type())
        {
        case QEvent::MouseButtonDblClick:
            plot->legendDoubleClicked(clickedItem);
            break;
        case QEvent::MouseButtonPress:
            plot->legendClicked(clickedItem);
            break;
        default:
            // Unhandled event type
            result = false;
            break;
        }
    }

    return result;
}
