#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include <QMouseEvent>
#include <QWheelEvent>

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_legend.h>
#include <qwt_plot_marker.h>

#include "plot_curve.hpp"


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


class PlotWidget : public QwtPlot
{
    Q_OBJECT

public:
    PlotWidget();
    virtual ~PlotWidget();

    virtual void updateLayout(void) override;

signals:
    // Emitted whenever the view rect is changed
    void viewChanged(const QRectF &viewrect);

    void cursorPositionChanged(double &t, double &y1, double &y2);

public slots:
    int getHorizontalPixels(void) const;

    bool addSeries(QSharedPointer<DataSeries> series, int axis_id = QwtPlot::yLeft);
    bool addSeries(DataSeries *series) { return addSeries(QSharedPointer<DataSeries>(series)); }

    bool removeSeries(QSharedPointer<DataSeries> series);
    bool removeSeries(DataSeries *series) { return removeSeries(QSharedPointer<DataSeries>(series)); }
    bool removeSeries(QString label);

    void autoScale(int axis_id = yBoth);

    void setBackgroundColor(QColor color);

protected slots:

    void legendClicked(const QVariant &item_info, int index);

protected:
    // Mouse actions
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    // Drag-n-drop actions
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    void initZoomer(void);
    void initPanner(void);
    void initLegend(void);
    void initCrosshairs(void);

    void resampleCurves(int axis_id=yBoth);

    // Curve tracking
    bool isCurveTracked(QSharedPointer<PlotCurve> curve);
    void trackCurve(QSharedPointer<PlotCurve> curve);
    void untrackCurve(void);

    QwtPlotZoomer *zoomer;
    PlotPanner *panner;
    QwtLegend *legend;

    QwtPlotMarker *crosshair;
    QwtPlotMarker *curveTracker;

    // List of curves attached to this widget
    QList<QSharedPointer<PlotCurve>> curves;

    QSharedPointer<PlotCurve> tracking_curve;

    // Specify "both" y axes
    static const int yBoth = -1;
};

#endif // PLOT_WIDGET_HPP
