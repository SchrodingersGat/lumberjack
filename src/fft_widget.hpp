#ifndef FFT_WIDGET_HPP
#define FFT_WIDGET_HPP

#include <qwt_plot.h>

#include "fft/include/simple_fft/fft_settings.h"
#include "fft/include/simple_fft/fft.h"

class FFTWidget : public QwtPlot
{
    Q_OBJECT

public:
    FFTWidget();
    virtual ~FFTWidget();

signals:

public slots:

protected:
};

#endif // FFT_WIDGET_HPP
