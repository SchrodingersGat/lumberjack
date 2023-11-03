#include <QDebug>

#include <qpen.h>
#include <qmath.h>

#include <qwt_text.h>
#include <qwt_plot_curve.h>

#include "fft_widget.hpp"


FFTWidget::FFTWidget() : PlotWidget()
{
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
    return new FFTCurveUpdater(*series);
}
