#include <qelapsedtimer.h>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include <qwt_text_label.h>
#include <qfont.h>

#include "plot_curve.hpp"



/**
 * @brief PlotCurve::PlotCurve - Create a new managed curve
 * @param s - the source data series
 * @param updater - the worker which resamples the data
 */
PlotCurve::PlotCurve(QSharedPointer<DataSeries> s, PlotCurveUpdater *updater) : QwtPlotCurve(), series(s)
{

    // Create a new worker thread
    if (updater == nullptr)
    {
        updater = new PlotCurveUpdater(*s);
    }

    worker = updater;

    if (!series.isNull())
    {
        QwtPlotCurve::setTitle((*series).getLabel());

        connect(&(*series), &DataSeries::styleUpdated, this, &PlotCurve::updateLineStyle);
        connect(&(*series), &DataSeries::styleUpdated, this, &PlotCurve::updateLabel);

#ifdef CI_UNIT_TEST
        qDebug() << "Skipping GUI steps for unit testing";
#else
        updateLineStyle();
#endif
    }

    worker->moveToThread(&workerThread);
    connect(worker, &PlotCurveUpdater::sampleComplete, this, &PlotCurve::onDataResampled);
}


PlotCurve::~PlotCurve()
{
    qDebug() << "Deleting PlotCurve";

    // Wait for the resampling thread to complete
    workerThread.quit();
    workerThread.wait();

    // Delete the worker thread
    delete worker;

    qDebug() << "~PlotCurve() complete";
}


void PlotCurve::onDataResampled(const QVector<double> &t_data, const QVector<double> &y_data)
{
    // Re-draw the curve
    setSamples(t_data, y_data);
}


void PlotCurve::resampleData(double t_min, double t_max, unsigned int n_pixels)
{
    if (!series.isNull() && worker != nullptr)
    {
        worker->updateCurveSamples(t_min, t_max, n_pixels);
    }
    else if (series.isNull())
    {
        qDebug() << "resampleData:" << "series is null";
    } else if (worker == nullptr) {
        qDebug() << "resampleData:" << "worker is null";
    }
}


void PlotCurve::setVisible(bool on)
{
    QwtPlotCurve::setVisible(on);
    updateLabel();
}


/*
 * Update the QwtText label used in the legend
 */
void PlotCurve::updateLabel()
{
    QwtText label;

    QFont font = label.font();

    if (isVisible())
    {
        font.setItalic(false);
        label.setColor(QColor(0x30, 0x30, 0x30));
    }
    else
    {
        font.setItalic(true);
        label.setColor(QColor(0xA0, 0xA0, 0xA0));
    }

    label.setFont(font);
    label.setText(series->getLabel());

    setTitle(label);
}


void PlotCurve::updateLineStyle()
{
    if (series.isNull())
    {
        return;
    }

    QPen pen(series->getColor());

    pen.setStyle((Qt::PenStyle) series->getLineStyle());

    pen.setWidthF(series->getLineWidth());

    setPen(pen);

    QwtSymbol* symbol = new QwtSymbol();

    symbol->setColor(series->getColor());
    symbol->setPen(pen);

    int s = series->getSymbolSize();
    symbol->setSize(QSize(s, s));

    symbol->setStyle((QwtSymbol::Style) series->getSymbolStyle());

    setSymbol(symbol);


}
