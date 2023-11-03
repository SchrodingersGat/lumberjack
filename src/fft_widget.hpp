#ifndef FFT_WIDGET_HPP
#define FFT_WIDGET_HPP

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>

#include "plot_widget.hpp"

#include "fft/include/simple_fft/fft_settings.h"
#include "fft/include/simple_fft/fft.h"

#include "data_series.hpp"
#include "plot_curve.hpp"
#include "plot_panner.hpp"


class FFTCurveUpdater : public PlotCurveUpdater
{
    Q_OBJECT

public:
    FFTCurveUpdater(DataSeries &data_series);

public slots:
    virtual void updateCurveSamples(double t_min, double t_max, unsigned int n_pixels) override;
};



class FFTWidget : public PlotWidget
{
    Q_OBJECT

public:
    FFTWidget();
    virtual ~FFTWidget();

protected:
    virtual bool isCurveTrackingEnabled(void) const override { return false; }
    virtual PlotCurveUpdater* generateNewWorker(QSharedPointer<DataSeries> series) override;
};

#endif // FFT_WIDGET_HPP
