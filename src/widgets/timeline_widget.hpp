#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include <qwt_plot_item.h>
#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_plot_zoomer.h>


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



/**
 * @brief The TimelineZoomer class is a custom implementation of QwtPlotZoomer
 *
 * Instead of applying the "zoom" to the timeline widget,
 * it passes the zoom information back to the main plot widget.
 */
class TimelineZoomer : public QwtPlotZoomer
{
public:
    explicit TimelineZoomer(QWidget *parent);

    virtual void rescale() override {};

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

    void onZoomed(const QRectF &zoomRect);

signals:
    void timeUpdated(const QwtInterval &interval);

protected:
    RangeMarker rangeMarker;

    TimelineZoomer *zoomer = nullptr;
};

#endif // TIMELINE_WIDGET_HPP
