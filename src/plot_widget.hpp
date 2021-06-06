#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include <QMouseEvent>
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

    virtual void updateLayout(void) override;

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
    void onViewPanned(int dx, int dy);

protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void initZoomer(void);
    void initPanner(void);

    void resampleCurves(void);

    QwtPlotZoomer *zoomer;
    QwtPlotPanner *panner;

    // List of curves attached to this widget
    QList<QSharedPointer<PlotCurve>> curves;
};

#endif // PLOT_WIDGET_HPP
