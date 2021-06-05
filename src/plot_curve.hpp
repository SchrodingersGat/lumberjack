#ifndef PLOT_CURVE_H
#define PLOT_CURVE_H

#include <qthread.h>

#include <qwt_plot_curve.h>

#include "data_series.hpp"

/*
class PlotCurve;

class PlotCurveUpdater : public QObject
{
    Q_OBJECT

public slots:
    PlotCurveUpdater(PlotCurve &curve) : plot_curve(curve) {}

    void updateCurveSamples(double t_min, double t_max, unsigned int n_pixels);

protected:
    PlotCurve &plot_curve;
};
*/

/*
 * An extension of the QwtPlotCurve class,
 * which links directly to a TimeSeries object.
 *
 * Includes intelligent down-sampling functionality,
 * so that curve features are retained at low zoom levels.
 *
 * A background thread performs the task of re-sampling data (as required).
 *
 */
class PlotCurve : public QObject, QwtPlotCurve
{
   Q_OBJECT

public:
    PlotCurve(QSharedPointer<DataSeries> series);
    PlotCurve(DataSeries* series) : PlotCurve(QSharedPointer<DataSeries>(series)) {}

    virtual ~PlotCurve();

    bool isSampling(void) const { return is_sampling;  }

public slots:
    void resampleData(double t_min, double t_max, unsigned int n_pixels);

protected:
    QSharedPointer<DataSeries> series;

    //! Thread for re-sampling curve data
    QThread samplerThread;

    //! Mutext eo
    mutable QMutex samplerMutex;

    bool is_sampling = false;
};

#endif // PLOT_CURVE_H
