#include <QDebug>

#include <qwt_text.h>

#include "fft_widget.hpp"

#define __USE_SQUARE_BRACKETS_FOR_ELEMENT_ACCESS_OPERATOR

#include "fft/include/simple_fft/fft_settings.h"
#include "fft/include/simple_fft/fft.h"


typedef std::vector<real_type> RealArray1D;
typedef std::vector<complex_type> ComplexArray1D;




FFTWidget::FFTWidget() : QwtPlot()
{
    initAxes();

    const char* error;

    const int N = 8192;

    RealArray1D data_in(N);
    ComplexArray1D data_out(N);

    bool res = simple_fft::FFT<RealArray1D, ComplexArray1D>(data_in, data_out, N, error);

    qDebug() << "FFT:" << N << res;
}


FFTWidget::~FFTWidget()
{
    // TODO
}


void FFTWidget::initAxes()
{
    QwtText title = axisTitle(QwtPlot::xBottom);

    auto font = title.font();
    font.setPointSize(8);
    title.setFont(font);

    title.setText("Frequency [Hz]");

    setAxisTitle(QwtPlot::xBottom, title);
}
