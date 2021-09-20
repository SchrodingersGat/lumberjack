#include "timeline_widget.hpp"

#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_widget.h>

#include <qdebug.h>


TimelineWidget::TimelineWidget(QWidget *parent) : QwtPlot(parent)
{
    setWindowTitle(tr("Timeline"));
    enableAxis(QwtPlot::xBottom, true);
    enableAxis(QwtPlot::yLeft, true);
    enableAxis(QwtPlot::yRight, false);

    setAxisScale(QwtPlot::yLeft, 0, 1);
    setAxisScale(QwtPlot::xBottom, 0, 1);

    setMinimumHeight(50);
    setMaximumHeight(100);

    // Disable drawing of left axis
    axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Ticks, false);

    // Disable drawing of bottom axis
    axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Ticks, false);
}


TimelineWidget::~TimelineWidget()
{
}


/**
 * @brief TimelineWidget::updateTimeLimits - set the min / max time limits
 * @param limits
 */
void TimelineWidget::updateTimeLimits(const QwtInterval &limits)
{
    setAxisScale(QwtPlot::xBottom, limits.minValue(), limits.maxValue());

    qDebug() << "updateTimeLimits" << limits;

    replot();
}


/**
 * @brief TimelineWidget::updateViewLimits - set the currently viewed range
 * @param limits
 */
void TimelineWidget::updateViewRect(const QRectF &viewRect)
{
    // TODO

    qDebug() << "updateView" << viewRect;

    replot();
}
