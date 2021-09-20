#include <qpainter.h>
#include <qrect.h>

#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_map.h>

#include "timeline_widget.hpp"


RangeMarker::RangeMarker()
{

}


void RangeMarker::setInterval(const QwtInterval &interval)
{
    this->interval = interval;
}


void RangeMarker::draw(QPainter *painter,
                       const QwtScaleMap &xMap,
                       const QwtScaleMap &yMap,
                       const QRectF &canvasRect) const
{
    int x1 = qRound(xMap.transform(interval.minValue()));
    int x2 = qRound(xMap.transform(interval.maxValue()));

    int y1 = canvasRect.top() + 5;
    int y2 = canvasRect.height() - 10;

    painter->fillRect(QRect(x1, y1, x2 - x1, y2), QColor(50, 50, 50, 75));

    QPen pen(QColor(50, 50, 50, 100));

    pen.setWidth(1);
    painter->setPen(pen);

    painter->drawRect(x1, y1, x2 - x1, y2);
}


TimelineZoomer::TimelineZoomer(QWidget *parent) : QwtPlotZoomer(parent, true)
{
    setMaxStackDepth(-1);
    setTrackerMode(QwtPicker::AlwaysOff);

    // Configure mouse actions
    setMousePattern(QwtPlotZoomer::MouseSelect2, Qt::MouseButton::NoButton);
    setMousePattern(QwtPlotZoomer::MouseSelect3, Qt::MouseButton::NoButton);
    setMousePattern(QwtPlotZoomer::MouseSelect4, Qt::MouseButton::NoButton);
    setMousePattern(QwtPlotZoomer::MouseSelect5, Qt::MouseButton::NoButton);
    setMousePattern(QwtPlotZoomer::MouseSelect6, Qt::MouseButton::NoButton);

    setZoomBase();
}



TimelineWidget::TimelineWidget(QWidget *parent) : QwtPlot(parent)
{
    setWindowTitle(tr("Timeline"));
    enableAxis(QwtPlot::xBottom, true);
    enableAxis(QwtPlot::yLeft, true);
    enableAxis(QwtPlot::yRight, false);

    setAxisScale(QwtPlot::yLeft, 0, 1);
    setAxisScale(QwtPlot::xBottom, 0, 1);

    setMinimumHeight(20);
    setMaximumHeight(100);

    // Disable drawing of left axis
    axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Ticks, false);

    // Disable drawing of bottom axis
    axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Ticks, false);

    // Initialize range marker rectangle
    rangeMarker.attach(this);
    rangeMarker.setInterval(QwtInterval(0, 1));

    // Initialize mouse zoomer
    zoomer = new TimelineZoomer(canvas());

    connect(zoomer, &QwtPlotZoomer::zoomed, this, &TimelineWidget::onZoomed);

}


TimelineWidget::~TimelineWidget()
{
    zoomer->deleteLater();
}


void TimelineWidget::onZoomed(const QRectF &zoomRect)
{
    emit timeUpdated(QwtInterval(zoomRect.left(), zoomRect.right()));
}


/**
 * @brief TimelineWidget::updateTimeLimits - set the min / max time limits
 * @param limits
 */
void TimelineWidget::updateTimeLimits(const QwtInterval &limits)
{
    setAxisScale(QwtPlot::xBottom, limits.minValue(), limits.maxValue());

    replot();
}


/**
 * @brief TimelineWidget::updateViewLimits - set the currently viewed range
 * @param limits
 */
void TimelineWidget::updateViewLimits(const QwtInterval &limits)
{
    rangeMarker.setInterval(limits);

    replot();
}
