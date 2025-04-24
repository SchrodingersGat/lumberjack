#include "plot_marker.hpp"

PlotMarker::PlotMarker(QSharedPointer<PlotCurve> plotCurve)
    : QwtPlotMarker(), curve(plotCurve)
{

}
