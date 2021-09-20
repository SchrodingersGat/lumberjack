#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include <qwt_plot.h>
#include <qwt_interval.h>


class TimelineWidget : public QwtPlot
{
    Q_OBJECT

public:
    TimelineWidget(QWidget *parent = nullptr);
    virtual ~TimelineWidget();

public slots:
    void updateTimeLimits(const QwtInterval &limits);
    void updateViewRect(const QRectF &viewRect);
};

#endif // TIMELINE_WIDGET_HPP
