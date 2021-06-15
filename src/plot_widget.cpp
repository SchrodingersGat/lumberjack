#include <QApplication>
#include <QMimeData>

#include "data_source.hpp"
#include "plot_widget.hpp"


PlotWidget::PlotWidget() : QwtPlot()
{
    // Enable secondary axis
    enableAxis(QwtPlot::yRight, true);

    setMinimumWidth(50);
    setMinimumHeight(50);

    auto policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    policy.setHorizontalStretch(1);
    policy.setVerticalStretch(1);
    setSizePolicy(policy);

    initZoomer();
    initPanner();
    initLegend();
    initCrosshairs();

    setAcceptDrops(true);
}

PlotWidget::~PlotWidget()
{
    untrackCurve();

    delete zoomer;

    crosshair->detach();
    delete crosshair;

    curveTracker->detach();
    delete curveTracker;
}


/**
 * @brief PlotWidget::dragEnterEvent is called when a drag event enters the widget
 * @param event
 */
void PlotWidget::dragEnterEvent(QDragEnterEvent *event)
{
    auto *mime = event->mimeData();

    // DataSeries is being dragged onto this PlotWidget
    if (mime->hasFormat("source") && mime->hasFormat("series"))
    {
        event->accept();
    }
}


/**
 * @brief PlotWidget::dragMoveEvent is called when an accepted drag event moves across the widget
 * @param event
 */
void PlotWidget::dragMoveEvent(QDragMoveEvent *event)
{
    // TODO
}


/**
 * @brief PlotWidget::dropEvent is called when an accepted drag event is dropped on the widget
 * @param event
 */
void PlotWidget::dropEvent(QDropEvent *event)
{
    auto *mime = event->mimeData();

    auto *manager = DataSourceManager::getInstance();

    // DataSeries is dropped onto this PlotWidget
    if (mime->hasFormat("source") && mime->hasFormat("series"))
    {
        QString source_lbl = mime->data("source");
        QString series_lbl = mime->data("series");

        auto series = manager->findSeries(source_lbl, series_lbl);

        if (series.isNull()) return;

        addSeries(series);
        event->accept();
    }
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


void PlotWidget::initLegend()
{
    legend = new QwtLegend();

    legend->setDefaultItemMode(QwtLegendData::Clickable);

    insertLegend(legend, QwtPlot::TopLegend);

    connect(legend, SIGNAL(clicked(const QVariant, int)), this, SLOT(legendClicked(const QVariant, int)));
}


/*
 * Configure crosshairs which track the mouse cursor,
 * and (optionally) a selected curve
 */
void PlotWidget::initCrosshairs()
{
    setMouseTracking(true);
    canvas()->setMouseTracking(true);

    // Configure a crosshair marker which will follow the mouse cursor
    crosshair = new QwtPlotMarker();
    crosshair->setLineStyle(QwtPlotMarker::Cross);
    crosshair->setZ(100);

    QPen pen;
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    pen.setColor(QColor(150, 150, 150));

    crosshair->setLinePen(pen);
    crosshair->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    crosshair->attach(this);

    curveTracker = new QwtPlotMarker();
    curveTracker->setLineStyle(QwtPlotMarker::HLine);
    curveTracker->setZ(101);

    curveTracker->setLinePen(pen);
    curveTracker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    curveTracker->attach(this);
    curveTracker->setVisible(false);
}


void PlotWidget::updateLayout()
{
    QwtPlot::updateLayout();

    // Force re-sampling of attached curve data when canvas dimensions are changed
    resampleCurves();
}


/*
 * Set the background color for the plot.
 *
 * Also updates the color of the crosshair,
 * ensuring it always stands out against the background.
 */
void PlotWidget::setBackgroundColor(QColor color)
{
    QBrush b = canvasBackground();

    b.setColor(color);

    setCanvasBackground(b);

    // TODO - Update the color of the crosshair
}


/*
 * Resample all attached curves to the displayed time region
 *
 * The y-axis can be specified if only one axis is to be updated
 */
void PlotWidget::resampleCurves(int axis_id)
{
    bool axis_left = (axis_id == yBoth) || (axis_id == QwtPlot::yLeft);
    bool axis_right = (axis_id == yBoth) || (axis_id == QwtPlot::yRight);

    auto interval = axisInterval(QwtPlot::xBottom);

    int n_pixels = getHorizontalPixels();

    for (auto curve : curves)
    {
        if (curve.isNull()) continue;

        int axis = curve->yAxis();

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


void PlotWidget::legendClicked(const QVariant &item_info, int index)
{
    auto modifiers = QApplication::keyboardModifiers();
    auto buttons = QApplication::mouseButtons();

    QwtPlotItem* item = qvariant_cast<QwtPlotItem*>(item_info);

    for (auto curve : curves)
    {
        if (!curve.isNull() && (QwtPlotItem*) &(*curve) == item)
        {
            auto series = curve->getDataSeries();

            if (series.isNull()) continue;

            if (modifiers == Qt::ShiftModifier)
            {
                // Toggle curve visibility
                curve->setVisible(!curve->isVisible());
            }
            else if (modifiers == Qt::ControlModifier)
            {
                // Remove the data series
                removeSeries(series);
            }
            else if (modifiers == Qt::AltModifier)
            {
                // Switch curve to the other axis
                int axis = curve->yAxis();

                curve->setYAxis(axis == QwtPlot::yLeft ? QwtPlot::yRight : QwtPlot::yLeft);
            }
            else
            {
                // Track?
            }
        }
    }

    setAutoReplot(false);
    replot();
}


/**
 * @brief PlotWidget::isCurveTracked checks if this PlotWidget is tracking a curve
 * @return
 */
bool PlotWidget::isCurveTracked()
{
    return !tracking_curve.isNull();
}


/**
 * @brief PlotWidget::isCurveTracked tests if the provided curve is being tracked
 * @param curve
 * @return
 */
bool PlotWidget::isCurveTracked(QSharedPointer<PlotCurve> curve)
{
    return !curve.isNull() && curve == tracking_curve;
}


/**
 * @brief PlotWidget::trackCurve causes the provided curve to be tracked by the cursor
 * @param curve
 */
void PlotWidget::trackCurve(QSharedPointer<PlotCurve> curve)
{
    untrackCurve();

    if (!curve.isNull())
    {
        tracking_curve = curve;
    }
}


/**
 * @brief PlotWidget::untrackCurve removes curve tracking association for this widget
 */
void PlotWidget::untrackCurve()
{
    tracking_curve = QSharedPointer<PlotCurve>(nullptr);
}


void PlotWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (delta == 0) return;

    // Prevent accidental zoom in-out while panning
    if (event->buttons() & Qt::MiddleButton) return;

    bool zoom_vertical = true;
    bool zoom_horizontal = true;

    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        // Vertical zoom only!
        zoom_horizontal = false;
    }
    else if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
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

    // Ignore if mouse wheel is over the legend
    if (canvas_pos.y() < 0)
    {
        return;
    }

    // Cursor is over the left axis
    else if (canvas_pos.x() < 0)
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


void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Update crosshair

    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    double x = invTransform(QwtPlot::xBottom, canvas_pos.x());
    double y1 = invTransform(QwtPlot::yLeft, canvas_pos.y());
    double y2 = invTransform(QwtPlot::yRight, canvas_pos.y());

    crosshair->setXValue(x);
    crosshair->setYValue(y1);

    emit cursorPositionChanged(x, y1, y2);

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


void PlotWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

        int axis_id = yBoth;

        if (canvas_pos.y() < 0)
        {
            // Ignore if user clicked on the legend
            return;
        }
        else if (canvas_pos.x() < 0)
        {
            axis_id = QwtPlot::yLeft;
        }
        else if (canvas_pos.x() > canvas()->width())
        {
            axis_id = QwtPlot::yRight;
        }
        else if (canvas_pos.y() > canvas()->height())
        {
            axis_id = QwtPlot::xBottom;
        }

        autoScale(axis_id);
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

    curve->attach(this);
    curve->setYAxis(axis_id);

    curves.push_back(QSharedPointer<PlotCurve>(curve));

    auto interval = axisInterval(QwtPlot::xBottom);

    // Perform initial data sampling
    curve->resampleData(interval.minValue(), interval.maxValue(), getHorizontalPixels());

    setAutoReplot(true);
    replot();

    return true;
}


bool PlotWidget::removeSeries(QSharedPointer<DataSeries> series)
{
    for (int idx = 0; idx < curves.size(); idx++)
    {
        auto curve = curves.at(idx);

        if (isCurveTracked(curve))
        {
            untrackCurve();
        }

        if (!curve.isNull() && curve->getDataSeries() == series)
        {
            curves.removeAt(idx);
            replot();
            return true;
        }
    }

    return false;
}


/*
 * Remove the first attached curve which matches the provided label string
 */
bool PlotWidget::removeSeries(QString label)
{
    for (auto curve : curves)
    {
        if (!curve.isNull() && curve->getDataSeries()->getLabel() == label)
        {
            return removeSeries(curve->getDataSeries());
        }
    }

    return false;
}


void PlotWidget::autoScale(int axis_id)
{
    QwtInterval interval_bottom;
    QwtInterval interval_left;
    QwtInterval interval_right;

    bool update = false;
    bool update_left = false;
    bool update_right = false;

    for (auto curve : curves)
    {
        // Ignore hidden data curves
        if (curve.isNull() || !curve->isVisible()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull() || series->size() == 0) continue;

        interval_bottom |= QwtInterval(series->getOldestTimestamp(), series->getNewestTimestamp());

        QwtInterval y(series->getMinimumValue(), series->getMaximumValue());

        if (curve->yAxis() == QwtPlot::yLeft)
        {
            interval_left |= y;
            update_left = true;
        }
        else if (curve->yAxis() == QwtPlot::yRight)
        {
            interval_right |= y;
            update_right = true;
        }

        update = true;
    }

    // Return if no curves were available for updating
    if (!update) return;

    if (axis_id == QwtPlot::xBottom || axis_id == yBoth)
    {
        setAxisScale(QwtPlot::xBottom, interval_bottom.minValue(), interval_bottom.maxValue());
    }

    if (update_left && (axis_id == QwtPlot::yLeft || axis_id == yBoth))
    {
        setAxisScale(QwtPlot::yLeft, interval_left.minValue(), interval_left.maxValue());

        // No curves available on the right axis, so update to match
        if (!update_right && axis_id == yBoth)
        {
            setAxisScale(QwtPlot::yRight, interval_left.minValue(), interval_left.maxValue());
        }
    }

    if (update_right && (axis_id == QwtPlot::yRight || axis_id == yBoth))
    {
        setAxisScale(QwtPlot::yRight, interval_right.minValue(), interval_right.maxValue());

        // No curves available on the left axis, so update to match
        if (!update_left && axis_id == yBoth)
        {
            setAxisScale(QwtPlot::yLeft, interval_right.minValue(), interval_right.maxValue());
        }
    }

    setAutoReplot(true);
    replot();
}


int PlotWidget::getHorizontalPixels() const
{
    return width();
}
