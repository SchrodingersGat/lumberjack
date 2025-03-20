#ifndef PLOT_MARKER_H
#define PLOT_MARKER_H

#include <qwt_plot_marker.h>

#include "plot_curve.hpp"

/*
 * Custom subclass of QwtPlotMarker to keep track of which curve the marker is assigned to
 */

class PlotMarker : public QwtPlotMarker
{
public:
    PlotMarker(QSharedPointer<PlotCurve> plotCurve = nullptr);

    bool hasCurve() { return !curve.isNull(); }

    QSharedPointer<PlotCurve> curve;
};

#endif // PLOT_MARKER_H
