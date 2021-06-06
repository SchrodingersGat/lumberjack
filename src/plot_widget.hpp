#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include <QWheelEvent>

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

#include "plot_curve.hpp"


class PlotWidget : public QwtPlot
{
    Q_OBJECT

public:
    PlotWidget();
    virtual ~PlotWidget();

signals:
    // Emitted whenever the view rect is changed
    void viewChanged(const QRectF &viewrect);

public slots:
    int getHorizontalPixels(void) const;

    bool addSeries(QSharedPointer<DataSeries> series);
    bool addSeries(DataSeries *series) { return addSeries(QSharedPointer<DataSeries>(series)); }

    bool removeSeries(QSharedPointer<DataSeries> series);
    bool removeSeries(DataSeries *series) { return removeSeries(QSharedPointer<DataSeries>(series)); }

protected slots:
    void onViewChanged(const QRectF &viewrect);

protected:
    void wheelEvent(QWheelEvent *event);

    QwtPlotZoomer *zoomer;

    // List of curves attached to this widget
    QList<QSharedPointer<PlotCurve>> curves;
};

#endif // PLOT_WIDGET_HPP
