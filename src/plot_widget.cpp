#include <QApplication>

#include "plot_widget.hpp"


PlotWidget::PlotWidget() : QwtPlot()
{
    // Enable secondary axis
    enableAxis(QwtPlot::yRight, true);

    initZoomer();
    initPanner();

    legend = new QwtLegend();

    legend->setDefaultItemMode(QwtLegendData::Clickable);

    insertLegend(legend, QwtPlot::TopLegend);

    // Re-sample curve data when the plot is resized
//    connec(canvas(), SIGNAL(resized))
}

PlotWidget::~PlotWidget()
{
    delete zoomer;
    // TODO
}

/*
 * Initialize QwtPlotZoomer to handle mouse driven zoom events
 */
void PlotWidget::initZoomer()
{
    zoomer = new QwtPlotZoomer(canvas(), true);

    zoomer->setMaxStackDepth(20);
    zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    zoomer->setZoomBase();

    // Configure mouse actions
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect2, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect3, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect4, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect5, Qt::MouseButton::NoButton);
    zoomer->setMousePattern(QwtPlotZoomer::MouseSelect6, Qt::MouseButton::NoButton);

    zoomer->setZoomBase();
}


/*
 * Initialize QwtPlotPanner to handle mouse panning events
 */
void PlotWidget::initPanner()
{
    panner = new PlotPanner(canvas());
    panner->setMouseButton(Qt::MiddleButton);
}


void PlotWidget::updateLayout()
{
    QwtPlot::updateLayout();

    // Force re-sampling of attached curve data when canvas dimensions are changed
    resampleCurves();
}


/*
 * Resample all attached curves to the displayed time region
 *
 * The y-axis can be specified if only one axis is to be updated
 */
void PlotWidget::resampleCurves(int axis_id)
{
    bool axis_left = (axis_id == -1) || (axis_id == QwtPlot::yLeft);
    bool axis_right = (axis_id == -1) || (axis_id == QwtPlot::yRight);

    auto interval = axisInterval(QwtPlot::xBottom);

    int n_pixels = getHorizontalPixels();

    for (auto curve : curves)
    {
        if (curve.isNull()) continue;

        int axis = ((QwtPlotCurve*) &(*curve))->yAxis();

        if (axis == QwtPlot::yLeft && !axis_left)
        {
            continue;
        }

        if (axis == QwtPlot::yRight && !axis_right)
        {
            continue;
        }

        curve->resampleData(interval.minValue(), interval.maxValue(), n_pixels);
    }
}


void PlotWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (delta == 0) return;

    // Prevent accidental zoom in-out while panning
    if (event->buttons() & Qt::MiddleButton) return;

    bool zoom_vertical = true;
    bool zoom_horizontal = true;

    if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
    {
        // Vertical zoom only!
        zoom_horizontal = false;
    }
    else if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        // Horizontal zoom only
        zoom_vertical = false;
    }

    double factor = 1.25;

    if (delta > 0)
    {
        factor = 1 / factor;
    }

    // Map the mouse position to the underlying canvas,
    // otherwise the size of the displayed axes causes offset issues
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    // List of axes we can zoom
    QList<int> axes;

    // If the mouse position is over a particular axis, we will *only* zoom that axis!

    // Cursor is over the left axis
    if (canvas_pos.x() < 0)
    {
        axes.append(QwtPlot::yLeft);
    }

    // Cursor is over the right axis
    else if (canvas_pos.x() > canvas()->width())
    {
        axes.append(QwtPlot::yRight);
    }

    // Cursor is over the bottom axis
    else if (canvas_pos.y() > canvas()->height())
    {
        axes.append(QwtPlot::xBottom);
    }
    else
    {
        axes.append(QwtPlot::xBottom);
        axes.append(QwtPlot::yLeft);
        axes.append(QwtPlot::yRight);
    }

    for (int axis_id : axes)
    {
        if (!axisEnabled(axis_id)) continue;

        const auto canvas_map = canvasMap(axis_id);

        double v1 = canvas_map.s1();
        double v2 = canvas_map.s2();

        if (canvas_map.transformation())
        {
            v1 = canvas_map.transform(v1);
            v2 = canvas_map.transform(v2);
        }

        double c = 0;

        switch (axis_id)
        {
        case QwtPlot::xBottom:
            if (!zoom_horizontal) continue;

            c = canvas_map.invTransform(canvas_pos.x());
            break;
        case QwtPlot::yLeft:
        case QwtPlot::yRight:
            if (!zoom_vertical) continue;

            c = canvas_map.invTransform(canvas_pos.y());
            break;
        default:
            continue;
        }

        const double center = 0.5 * (v1 + v2);
        const double width_2 = 0.5 * (v2 - v1) * factor;
        const double new_center = c - factor * (c - center);

        v1 = new_center - width_2;
        v2 = new_center + width_2;

        if (canvas_map.transformation())
        {
            v1 = canvas_map.invTransform(v1);
            v2 = canvas_map.invTransform(v2);
        }

        setAxisScale(axis_id, v1, v2);

    }

    replot();
}


void PlotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::BackButton)
    {
        zoomer->zoom(-1);
    }
    else if (event->button() == Qt::ForwardButton)
    {
        zoomer->zoom(+1);
    }
}


bool PlotWidget::addSeries(QSharedPointer<DataSeries> series, int axis_id)
{
    if (series.isNull()) return false;

    // If a curve already exists for this DataSeries, ignore
    for (auto curve : curves)
    {
        if (!curve.isNull() && curve->getDataSeries() == series)
        {
            // TODO: Un-commenting this line causes a SEGFAULT - Need to investigate
            // qInfo() << "Curve already added for" << series->getLabel() << "- skipping";
            return false;
        }
    }

    // Create a new PlotCurve for the DataSeries
    PlotCurve *curve = new PlotCurve(series);

    ((QwtPlotCurve*) curve)->attach(this);
    ((QwtPlotCurve*) curve)->setYAxis(axis_id);

    curves.push_back(QSharedPointer<PlotCurve>(curve));

    auto interval = axisInterval(QwtPlot::xBottom);

    // Perform initial data sampling
    curve->resampleData(interval.minValue(), interval.maxValue(), getHorizontalPixels());

    return true;
}


bool PlotWidget::removeSeries(QSharedPointer<DataSeries> series)
{
    for (int idx = 0; idx < curves.size(); idx++)
    {
        auto curve = curves.at(idx);

        if (!curve.isNull() && curve->getDataSeries() == series)
        {
            curves.removeAt(idx);
            return true;
        }
    }

    return false;
}


int PlotWidget::getHorizontalPixels() const
{
    return width();
}
