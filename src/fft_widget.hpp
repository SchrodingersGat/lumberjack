#ifndef FFT_WIDGET_HPP
#define FFT_WIDGET_HPP

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>

#include "plot_widget.hpp"

#include "fft/include/simple_fft/fft_settings.h"
#include "fft/include/simple_fft/fft.h"

#include "plot_panner.hpp"

class FFTWidget : public PlotWidget
{
    Q_OBJECT

public:
    FFTWidget();
    virtual ~FFTWidget();

signals:

public slots:

protected:
    virtual bool isCurveTrackingEnabled(void) const override { return false; }
};

#endif // FFT_WIDGET_HPP
