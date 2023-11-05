#include <qmath.h>
#include <qglobal.h>
#include <qelapsedtimer.h>

#include "fft_sampler.hpp"

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
    Q_UNUSED(n_pixels);

    QElapsedTimer fft_timer;
    fft_timer.restart();

    // Initialize empty arrays
    QVector<double> x_data;
    QVector<double> y_data;

    if (series.size() == 0)
    {
        emit sampleComplete(x_data, y_data);
        return;
    }

    // If the arguments are the same as last time, ignore
    if (t_min == t_min_latest && t_max == t_max_latest)
    {
        return;
    }

    t_min_latest = t_min;
    t_max_latest = t_max;


    // Extract *all* data between the required points
    auto idx_min = series.getIndexForTimestamp(t_min);
    auto idx_max = series.getIndexForTimestamp(t_max);

    if (idx_min < 0) idx_min = 0;
    if (idx_max >= series.size()) idx_max = series.size() - 1;

    // Recalculate endpoint timestamps
    t_min = series.getDataPoint(idx_min).timestamp;
    t_max = series.getDataPoint(idx_max).timestamp;

    auto n_samples = idx_max - idx_min;

    // TODO: Implement a data-pruning algorithm when there are too many samples
    const uint64_t MAX_FFT_SAMPLES = 0x10000;
    const uint64_t MIN_FFT_SAMPLES = 0x80;

    if (n_samples < MIN_FFT_SAMPLES)
    {
        emit sampleComplete(x_data, y_data);
        return;
    }

    // For the FFT to work, we assume that the samples are at fixed frequency
    double timespan = qAbs<double>(t_max - t_min);
    double dt = timespan / n_samples;

    // Calculate the "maximum" frequency we can measure
    double f_max = 0.5 / dt;

    uint64_t N = 2;

    // Calculate a "power of 2" sample size
    while (N < n_samples && N < MAX_FFT_SAMPLES)
    {
        N <<= 1;
    }

    RealArray1D data_in(N);
    ComplexArray1D data_out(N);

    // Copy across the data
    for (uint64_t ii = 0; ii < N; ii++)
    {
        uint64_t wrapped_idx = ii % n_samples;

        // If we have to pad out the data, wrap it around on itself
        data_in[ii] = series.getDataPoint(idx_min + wrapped_idx).value;
    }

    const char* error;

    bool result = simple_fft::FFT<RealArray1D, ComplexArray1D>(data_in, data_out, N, error);


    if (!result)
    {
        qWarning() << "Error calculating FFT data:" << QString(error);
        emit sampleComplete(x_data, y_data);
        return;
    }

    double y_max = 0;

    RealArray1D real_out(N);

    x_data.reserve(N);
    y_data.reserve(N);

    // Calculate max values to normalize
    for (uint64_t ii = 0; ii < N; ii++)
    {
        double real = data_out[ii].real();
        double imag = data_out[ii].imag();

        double out = qSqrt(real*real + imag*imag);

        real_out[ii] = out;

        // Prevent the DC value from swamping the overall data
        if (ii > 0)
        {
            y_max = qMax<double>(y_max, out);
        }
    }

    for (uint64_t jj = 0; jj < N/2; jj++)
    {
        double f = jj / dt / N;

        x_data.append(f);
        y_data.append(real_out[jj] / y_max);
    }

    emit sampleComplete(x_data, y_data);

    // qDebug() << "Completed FFT for" << N << "samples in" << QString::number(fft_timer.elapsed(), 'f', 3) << "ms";

}
