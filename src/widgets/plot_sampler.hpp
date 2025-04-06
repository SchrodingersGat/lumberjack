#ifndef PLOT_SAMPLER_HPP
#define PLOT_SAMPLER_HPP

#include <QMutex>

#include "data_series.hpp"


/*
 * Class which manages curve resampling.
 * Curve sampling is handled in a background thread, using this class to sample data.
 */
class PlotCurveUpdater : public QObject
{
    Q_OBJECT

public:
    PlotCurveUpdater(DataSeries &data_series);

public slots:
    virtual void updateCurveSamples(double t_min, double t_max, unsigned int n_pixels);

signals:
    // Sampled data is returned
    void sampleComplete(const QVector<double>&, const QVector<double>&);

protected:
    DataSeries &series;

    //! Mutex to prevent simultaneous sampling
    mutable QMutex mutex;

    double t_min_latest = -1;
    double t_max_latest = -1;
    unsigned int n_pixels_latest = 0;
};


#endif // PLOT_SAMPLER_HPP
