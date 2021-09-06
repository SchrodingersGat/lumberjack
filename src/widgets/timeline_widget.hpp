#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include <qwt_plot.h>

class TimelineWidget : public QwtPlot
{
    Q_OBJECT

public:
    TimelineWidget(QWidget *parent = nullptr);
    virtual ~TimelineWidget();
};

#endif // TIMELINE_WIDGET_HPP
