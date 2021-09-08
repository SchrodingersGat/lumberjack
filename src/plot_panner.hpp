#ifndef PLOT_PANNER_HPP
#define PLOT_PANNER_HPP

#include <QMouseEvent>

#include <qwt_plot_panner.h>


/*
 * Custom subclass of QwtPlotPanner,
 * which supports "continuous replot" action when panning.
 * Ref: https://stackoverflow.com/questions/14747959/qwt-zoomer-plus-panner-with-continuous-replot
 */
class PlotPanner : public QwtPlotPanner
{
public:
    explicit PlotPanner(QWidget *parent) : QwtPlotPanner(parent) {}

    virtual bool eventFilter(QObject *object, QEvent *event) override
    {
        if (!object || object != parentWidget()) return false;

        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);

            if (me->buttons() & Qt::MiddleButton)
            {
                widgetMouseMoveEvent(me);
                widgetMouseReleaseEvent(me);
                setMouseButton(me->button(), me->modifiers());
                widgetMousePressEvent(me);

                return false;
            }
        }

        return QwtPlotPanner::eventFilter(object, event);
    }
};

#endif // PLOT_PANNER_HPP
