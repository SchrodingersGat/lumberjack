#include "timeline_widget.hpp"


TimelineWidget::TimelineWidget(QWidget *parent) : QwtPlot(parent)
{
    // TODO

    setWindowTitle("Timeline");
    enableAxis(QwtPlot::xBottom, false);
    enableAxis(QwtPlot::yLeft, false);
    enableAxis(QwtPlot::yRight, false);
}


TimelineWidget::~TimelineWidget()
{
    // TODO
}
