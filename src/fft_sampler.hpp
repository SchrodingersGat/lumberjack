#ifndef FFT_SAMPLER_H
#define FFT_SAMPLER_H

#include "plot_sampler.hpp"

class FFTCurveUpdater : public PlotCurveUpdater
{
    Q_OBJECT

public:
    FFTCurveUpdater(DataSeries &data_series);

public slots:
    virtual void updateCurveSamples(double t_min, double t_max, unsigned int n_pixels) override;
};


#endif // FFT_SAMPLER_H
