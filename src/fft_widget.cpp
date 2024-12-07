#include <QDebug>

#include <qpen.h>
#include <qmath.h>

#include <qwt_text.h>
#include <qwt_text_label.h>
#include <qwt_plot_curve.h>

#include "plot_curve.hpp"
#include "fft_sampler.hpp"
#include "fft_widget.hpp"


FFTWidget::FFTWidget() : PlotWidget()
{
    initAxes();
}


FFTWidget::~FFTWidget()
{
    // TODO
}


void FFTWidget::initAxes()
{
    enableAxis(QwtPlot::yRight, false);

    auto label = axisTitle(QwtPlot::xBottom);
    auto font = label.font();

    font.setPointSize(8);
    label.setFont(font);
    label.setText("Frequency [Hz]");

    setAxisTitle(QwtPlot::xBottom, label);
    setAxisScale(QwtPlot::yLeft, 0, 1);
}


/**
 * @brief PlotWidget::generateNewWorker - Create a new curve sampling worker
 * @return
 */
PlotCurveUpdater* FFTWidget::generateNewWorker(QSharedPointer<DataSeries> series)
{
    return new FFTCurveUpdater(*series);
}


/*
 * Disable auto-resampling for FFT data.
 */
void FFTWidget::resampleCurves(int axis_id)
{
    Q_UNUSED(axis_id)

    for (auto curve : curves)
    {
        if (curve.isNull()) continue;
        curve->resampleData(timestamp_min, timestamp_max, 0);
    }

    setAxisScale(QwtPlot::yLeft, 0, 1);
}


/*
 * Update the FFT period limits when the visible interval changes
 */
void FFTWidget::updateInterval(const QwtInterval &interval)
{
    timestamp_min = interval.minValue();
    timestamp_max = interval.maxValue();

    resampleCurves(QwtPlot::xBottom);

}
