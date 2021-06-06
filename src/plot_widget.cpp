#include <QApplication>

#include "plot_widget.hpp"


PlotWidget::PlotWidget() : QwtPlot()
{
    // Enable secondary axis
    enableAxis(QwtPlot::yRight, true);

    initZoomer();
    initPanner();

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

    // connect(zoomer, SIGNAL(zoomed(const QRectF&)), this, SLOT(onViewChanged(const QRectF&)));
}


/*
 * Initialize QwtPlotPanner to handle mouse panning events
 */
void PlotWidget::initPanner()
{
    panner = new QwtPlotPanner(canvas());
    panner->setMouseButton(Qt::MiddleButton);

    connect(panner, SIGNAL(moved(int, int)), this, SLOT(onViewPanned(int, int)));
}


void PlotWidget::updateLayout()
{

    QwtPlot::updateLayout();

    resampleCurves();
}


/*
 * Resample all attached curves to the displayed time region
 */
void PlotWidget::resampleCurves()
{
    auto interval = axisInterval(QwtPlot::xBottom);

    int n_pixels = getHorizontalPixels();

    for (auto curve : curves)
    {
        if (curve.isNull()) continue;

        curve->resampleData(interval.minValue(), interval.maxValue(), n_pixels);
    }
}


void PlotWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

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

    for (int axis_id = 0; axis_id < QwtPlot::axisCnt; axis_id++)
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


void PlotWidget::onViewChanged(const QRectF &viewrect)
{
    Q_UNUSED(viewrect);
}


/*
 * Callback when the view is panned
 */
void PlotWidget::onViewPanned(int dx, int dy)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);

    // TODO: Push the current view rect onto the zoomer history stack
}


bool PlotWidget::addSeries(QSharedPointer<DataSeries> series)
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

    // Attach the curve to the plot
    ((QwtPlotCurve*) curve)->attach(this);

    curves.push_back(QSharedPointer<PlotCurve>(curve));

    auto interval = axisInterval(QwtPlot::xBottom);

    // Perform initial data sampling
    curve->resampleData(interval.minValue(), interval.maxValue(), getHorizontalPixels());

    qDebug() << "Added curve (" << curves.size() << ")";

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
