#include <QDebug>

#include <qpen.h>
#include <qmath.h>

#include <qwt_text.h>
#include <qwt_plot_curve.h>

#include "fft_widget.hpp"

#define __USE_SQUARE_BRACKETS_FOR_ELEMENT_ACCESS_OPERATOR

#include "fft/include/simple_fft/fft_settings.h"
#include "fft/include/simple_fft/fft.h"


typedef std::vector<real_type> RealArray1D;
typedef std::vector<complex_type> ComplexArray1D;


FFTCurveUpdater::FFTCurveUpdater(DataSeries &data_series) : PlotCurveUpdater(data_series)
{

}



/*
 * Custom curve updater method which calculates the FFT for the provided data.
 */
void FFTCurveUpdater::updateCurveSamples(double t_min, double t_max, unsigned int n_pixels)
{

    // TODO: Custom update function here
    PlotCurveUpdater::updateCurveSamples(t_min, t_max, n_pixels);

    return;

    // If the arguments are the same as last time, ignore
    if (t_min == t_min_latest && t_max == t_max_latest && n_pixels == n_pixels_latest)
    {
        return;
    }

    t_min_latest = t_min;
    t_max_latest = t_max;
    n_pixels_latest = n_pixels;

    // TODO: Calculate the FFT of the sample
}



FFTWidget::FFTWidget() : PlotWidget()
{
    /*
    return;

    initAxes();

    const char* error;

    const int N = 8192;

    RealArray1D time(N);

    RealArray1D data_in(N);
    ComplexArray1D data_out(N);

    RealArray1D real_out(N);
    RealArray1D im_out(N);

    for (int ii = 0; ii < N; ii++)
    {
        double t = ii * 0.001;
        double d = (double) qSin(t) + 2 * qCos(5 * t) + 100 * qCos(50 * t);

        time[ii] = t;
        data_in[ii] = d;
    }

    bool res = simple_fft::FFT<RealArray1D, ComplexArray1D>(data_in, data_out, N, error);

    double f_max = 0;

    for (int ii = 0; ii < N; ii++)
    {
        real_out[ii] = data_out[ii].real();

        f_max = qMax(f_max, qAbs(real_out[ii]));

        im_out[ii] = data_out[ii].imag();
    }

    qDebug() << "FFT:" << N << res << error;

    QwtPlotCurve *curve = new QwtPlotCurve("input");
    QwtPlotCurve *freq = new QwtPlotCurve("freq");
    QwtPlotCurve *im = new QwtPlotCurve("im");

    QVector<double> t_data = QVector<double>::fromStdVector(time);
    QVector<double> y_data = QVector<double>::fromStdVector(data_in);

    QVector<double> t_scaled = QVector<double>(N);
    QVector<double> f_data = QVector<double>(N);
//    QVector<double> i_data = QVector<double>::fromStdVector(im_out);


    double t_scaler = 1000.0f;

    for (int ii = 0; ii < N; ii++)
    {
        f_data[ii] = qAbs(real_out[ii]) / f_max;
        t_scaled[ii] = t_data[ii] * t_scaler;
    }

    curve->setSamples(t_data, y_data);
    curve->attach(this);

    freq->setSamples(t_scaled, f_data);
    freq->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    freq->attach(this);

    QPen pen = freq->pen();
    pen.setColor(QColor(50, 50, 250));

    freq->setPen(pen);

//    im->setSamples(t_data, i_data);
//    im->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
//    im->attach(this);


    zoomer = new QwtPlotZoomer(canvas(), true);

    zoomer->setMaxStackDepth(-1);
    zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    zoomer->setZoomBase();

    // Configure mouse actions
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect2, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect3, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect4, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect5, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect6, Qt::MouseButton::NoButton);

    zoomer->setZoomBase();

    panner = new PlotPanner(canvas());
    panner->setMouseButton(Qt::MiddleButton);
    */
}


FFTWidget::~FFTWidget()
{
    // TODO
}


/**
 * @brief PlotWidget::generateNewWorker - Create a new curve sampling worker
 * @return
 */
PlotCurveUpdater* FFTWidget::generateNewWorker(QSharedPointer<DataSeries> series)
{
    qDebug() << "generating new fft worker";
    return new FFTCurveUpdater(*series);
}
