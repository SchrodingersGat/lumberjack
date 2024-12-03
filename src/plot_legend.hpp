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

    virtual void updateLegend( const QwtPlotItem *item, const QList<QwtLegendData> &data) QWT_OVERRIDE;
};

#endif // PLOT_LEGEND_HPP
