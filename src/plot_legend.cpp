#include <QPen>


#include <qdebug.h>
#include <qwt_plot.h>
#include <qwt_text.h>
#include <qwt_legend_data.h>

#include "plot_legend.hpp"



PlotLegend::PlotLegend()
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
    // Prevent item addition if the yAxisId values do not match
    if (!item || item->yAxis() != yAxis()) return;
    QwtPlotLegendItem::updateLegend(item, data);
}
