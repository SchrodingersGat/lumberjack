#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include <qwt_plot_item.h>
#include <qwt_plot.h>
#include <qwt_interval.h>


/**
 * @brief The RangeMarker class is a custom QwtPlotItem to draw a rectangular section on the curve.
 *
 * This rectangle shows the current "viewport" of the timeseries data,
 * in relation to the available timescale.
 */
class RangeMarker : public QwtPlotItem
{
public:
    RangeMarker();

    void setInterval(const QwtInterval &interval);

    virtual void draw(QPainter *painter,
                      const QwtScaleMap &xMap,
                      const QwtScaleMap &yMap,
                      const QRectF &canvasRect) const override;

protected:
    QwtInterval interval;
};



class TimelineWidget : public QwtPlot
{
    Q_OBJECT

public:
    TimelineWidget(QWidget *parent = nullptr);
    virtual ~TimelineWidget();

public slots:
    void updateTimeLimits(const QwtInterval &limits);
    void updateViewLimits(const QwtInterval &limits);

protected:
    RangeMarker rangeMarker;
};

#endif // TIMELINE_WIDGET_HPP
