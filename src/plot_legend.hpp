#ifndef PLOT_LEGEND_HPP
#define PLOT_LEGEND_HPP

#include <QWidget>
#include <qwt_plot_legenditem.h>


class PlotLegend;
class PlotWidget;

class PlotLegend : public QObject, public QwtPlotLegendItem
{
    Q_OBJECT

public:
    PlotLegend(PlotWidget *plot);

    void setLineColor(const QColor &color);
    void setFillColor(const QColor &color);
    void setFontSize(int size);
    void setAxisAlignment(QwtAxisId axis);

    virtual void updateLegend( const QwtPlotItem *item, const QList<QwtLegendData> &data) QWT_OVERRIDE;

    bool handleMousePressEvent(const QMouseEvent *event);

protected:
    PlotWidget *plot = nullptr;
};

#endif // PLOT_LEGEND_HPP
