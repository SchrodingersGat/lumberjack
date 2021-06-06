#ifndef PLOT_CURVE_H
#define PLOT_CURVE_H

#include <qthread.h>

#include <qwt_plot_curve.h>

#include "data_series.hpp"

class PlotCurve;

class PlotCurveUpdater : public QObject
{
    Q_OBJECT

public:
    PlotCurveUpdater(DataSeries &data_series);

public slots:

    void updateCurveSamples(double t_min, double t_max, unsigned int n_pixels);

signals:
    // Sampled data is returned
    void sampleComplete(const QVector<double>&, const QVector<double>&);

protected:
    DataSeries &series;

    //! Mutex to prevent simultaneous sampling
    mutable QMutex mutex;

    bool is_sampling = false;
};

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

    QSharedPointer<DataSeries> getDataSeries(void) { return series; }


    virtual ~PlotCurve();

public slots:
    void resampleData(double t_min, double t_max, unsigned int n_pixels);

protected slots:
    void onDataResampled(const QVector<double> &t_data, const QVector<double> &y_data);

protected:
    QSharedPointer<DataSeries> series;

    PlotCurveUpdater worker;

    //! Thread for re-sampling curve data
    QThread workerThread;
};

#endif // PLOT_CURVE_H
