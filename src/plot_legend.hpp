#ifndef PLOT_LEGEND_HPP
#define PLOT_LEGEND_HPP

#include <QWidget>
#include <qwt_plot_legenditem.h>


class PlotLegend;


class PlotLegend : public QwtPlotLegendItem
{
public:
    PlotLegend();

    void setLineColor(const QColor &color);
    void setFillColor(const QColor &color);
    void setFontSize(int size);
    void setAxisAlignment(QwtAxisId axis);
};

#endif // PLOT_LEGEND_HPP
