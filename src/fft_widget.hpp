#ifndef FFT_WIDGET_HPP
#define FFT_WIDGET_HPP

#include "plot_widget.hpp"
#include "data_series.hpp"
#include "plot_curve.hpp"
#include "plot_panner.hpp"

#include "fft_sampler.hpp"


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
