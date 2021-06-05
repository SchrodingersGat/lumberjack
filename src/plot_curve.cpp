#include "plot_curve.hpp"

/*

PlotCurveUpdater::PlotCurveUpdater(PlotCurve &c) : curve(c)
{
    // TODO
}


void PlotCurveUpdater::updateCurveSamples(double t_min, double t_max, unsigned int n_pixels)
{
    // TODO
}
*/


PlotCurve::PlotCurve(QSharedPointer<DataSeries> s) :
    QwtPlotCurve(),
    series(s)
{
    if (!series.isNull())
    {
        // QwtPlotCurve::setTitle((*series).getLabel());
    }
}


PlotCurve::~PlotCurve()
{
    // Wait for the resampling thread to complete
    samplerThread.quit();
    samplerThread.wait();
}


void PlotCurve::resampleData(double t_min, double t_max, unsigned int n_pixels)
{
    // PlotCurveUpdater *updater = new PlotCurveUpdater(*this);

    //updater->moveToThread(samplerThread)
}



