#include <QPen>


#include <qevent.h>
#include <qdebug.h>
#include <qwt_plot.h>
#include <qwt_text.h>
#include <qwt_legend_data.h>
#include <qwt_plot_curve.h>

#include "plot_legend.hpp"
#include "plot_widget.hpp"


PlotLegend::PlotLegend(PlotWidget *plot) : plot(plot)
{
    // Initial rendering options
    setRenderHint(QwtPlotItem::RenderAntialiased);
    setLineColor(Qt::white);
    setFillColor(Qt::gray);
    setFontSize(12);

    setMaxColumns(1);
    setBackgroundMode(QwtPlotLegendItem::BackgroundMode::LegendBackground);
    setBorderRadius(8);
    setMargin(6);
    setSpacing(4);
    setItemMargin(3);
}


void PlotLegend::setLineColor(const QColor &color)
{
    setTextPen(color);
    setBorderPen(color);
}


void PlotLegend::setFillColor(const QColor &color)
{
    QColor c = color;
    c.setAlpha(150);

    setBackgroundBrush(c);
}


void PlotLegend::setFontSize(int size)
{
    QFont f = font();
    f.setPointSize(size);
    setFont(f);
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


bool PlotLegend::handleMousePressEvent(const QMouseEvent *event)
{
    if (!plot) return false;
    if (!event) return false;

    QRect rect = geometry(plot->geometry());

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

    if (clickedItem)
    {
        plot->legendClicked(clickedItem);
    }

    return true;
}
