#include <QApplication>
#include <QMimeData>
#include <qcolordialog.h>
#include <qmenu.h>
#include <qaction.h>
#include <qwt_scale_map.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qclipboard.h>
#include <qguiapplication.h>
#include <qfiledialog.h>

#include "axis_scale_dialog.hpp"

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
    initGrid();

    setAcceptDrops(true);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &PlotWidget::customContextMenuRequested, this, &PlotWidget::onContextMenu);
}

PlotWidget::~PlotWidget()
{
    untrackCurve();

    delete zoomer;

    crosshair->detach();
    delete crosshair;

    curveTracker->detach();
    delete curveTracker;

    grid->detach();
    delete grid;
}


void PlotWidget::onContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    // Fit / zoom submenu
    QMenu *fitMenu = new QMenu(tr("Fit"), &menu);

    QAction *fitToScreen = new QAction(tr("Fit all data"), fitMenu);
    QAction *fitX = new QAction(tr("Fit X Axis"), fitMenu);
    QAction *fitLeft = new QAction(tr("Fit Left Axis"), fitMenu);
    QAction *fitRight = new QAction(tr("Fit Right Axis"), fitMenu);

    fitMenu->addAction(fitToScreen);
    fitMenu->addAction(fitX);
    fitMenu->addAction(fitLeft);
    fitMenu->addAction(fitRight);

    menu.addMenu(fitMenu);

    // Grid submenu
    QMenu *gridMenu = new QMenu(tr("Grid"), &menu);

    QAction *toggleGridX = new QAction(tr("X Axis"), gridMenu);
    QAction *toggleGridY = new QAction(tr("Y Axis"), gridMenu);

    toggleGridX->setCheckable(true);
    toggleGridY->setCheckable(true);

    toggleGridX->setChecked(isXGridEnabled());
    toggleGridY->setChecked(isYGridEnabled());

    gridMenu->addAction(toggleGridX);
    gridMenu->addAction(toggleGridY);

    menu.addMenu(gridMenu);

    // Data menu
    QMenu *dataMenu = new QMenu(tr("Data"), &menu);

    QAction *imageToClipboard = new QAction(tr("Image to Clipboard"), dataMenu);
    QAction *imageToFile = new QAction(tr("Image to File"), dataMenu);

    QAction *clearAll = new QAction(tr("Clear All"), dataMenu);

    dataMenu->addAction(imageToClipboard);
    dataMenu->addAction(imageToFile);
    dataMenu->addSeparator();
    dataMenu->addAction(clearAll);

    menu.addMenu(dataMenu);

    // Plot submenu
    QMenu *plotMenu = new QMenu(tr("Plot"), &menu);

    QAction *bgColor = new QAction(tr("Set Color"), plotMenu);

    plotMenu->addAction(bgColor);

    menu.addMenu(plotMenu);

    QAction *action = menu.exec(mapToGlobal(pos));

    if (action == fitToScreen)
    {
        autoScale(yBoth);
    }
    else if (action == fitX)
    {
        autoScale(QwtPlot::xBottom);
    }
    else if (action == fitLeft)
    {
        autoScale(QwtPlot::yLeft);
    }
    else if (action == fitRight)
    {
        autoScale(QwtPlot::yRight);
    }
    else if (action == toggleGridX)
    {
        xGridEnable(!isXGridEnabled());
    }
    else if (action == toggleGridY)
    {
        yGridEnable(!isYGridEnabled());
    }
    else if (action == imageToClipboard)
    {
        saveImageToClipboard();
    }
    else if (action == imageToFile)
    {
        saveImageToFile();
    }
    else if (action == clearAll)
    {
        removeAllSeries();
    }
    else if (action == bgColor)
    {
        selectBackgroundColor();
    }
}


/**
 * @brief PlotWidget::setBackgroundColor launches a dialog to select the background color
 */
void PlotWidget::selectBackgroundColor()
{
    bool ok = false;

    QColor c = QColorDialog::getRgba(this->canvasBackground().color().rgb(), &ok);

    c.setAlpha(255);

    if (ok)
    {
        setBackgroundColor(c);
    }
}


void PlotWidget::saveImageToClipboard()
{
    auto *cliboard = QGuiApplication::clipboard();

    cliboard->setPixmap(grab());
}


void PlotWidget::saveImageToFile()
{
    auto image = grab().toImage();

    QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Save Screenshot"),
                "screenshot.png");

    if (!filename.isEmpty())
    {
        image.save(filename);
    }
}


void PlotWidget::setBackgroundColor(QColor color)
{
    QBrush b = canvasBackground();

    b.setColor(color);

    setCanvasBackground(b);

    // Ensure that the crosshair color is the inverse of the background

    QColor inverse = color.black() > 125 ? QColor(0xFF, 0xFF, 0xFF) : QColor(0x00, 0x00, 0x00);

    auto pen = crosshair->linePen();

    pen.setColor(inverse);

    crosshair->setLinePen(pen);

    replot();
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
        event->acceptProposedAction();
    }

    // Drag and drop local files
    if (event->mimeData()->hasUrls())
    {
        for (const QUrl& url : event->mimeData()->urls())
        {
            const QString& filename = url.toLocalFile();

            QFileInfo info(filename);

            if (info.exists() && info.isFile())
            {
                event->acceptProposedAction();
            }
        }
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
    else
    {
        for (const QUrl &url : event->mimeData()->urls())
        {
            const QString& filename = url.toLocalFile();

            QFileInfo info(filename);

            if (info.exists() && info.isFile())
            {
                emit fileDropped(filename);
            }
        }
    }
}



/*
 * Initialize QwtPlotZoomer to handle mouse driven zoom events
 */
void PlotWidget::initZoomer()
{
    zoomer = new QwtPlotZoomer(canvas(), true);

    zoomer->setMaxStackDepth(-1);
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


void PlotWidget::initGrid()
{
    grid = new QwtPlotGrid();

    grid->attach(this);

    grid->enableX(false);
    grid->enableY(false);
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
    Q_UNUSED(index);

    auto modifiers = QApplication::keyboardModifiers();

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
                trackCurve(curve);
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

    if (curve.isNull()) return;

    auto series = curve->getDataSeries();

    if (series.isNull() || series->size() == 0) return;

    tracking_curve = curve;
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


bool PlotWidget::handleMiddleMouseDrag(QMouseEvent *event)
{
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    if (!lastMousePosition.isNull())
    {
        QPoint delta = canvas_pos - lastMousePosition;

        int x_start = middleMouseStartPoint.x();
        int y_start = middleMouseStartPoint.y();

        if (x_start < 0)
        {
            // We are dragging the "left" axis
            panner->setAxisEnabled(QwtPlot::yRight, false);
            panner->moveCanvas(0, delta.y());
            panner->setAxisEnabled(QwtPlot::yRight, true);
        }
        else if (x_start > canvas()->width())
        {
            // We are dragging the "right" axis
            panner->setAxisEnabled(QwtPlot::yLeft, false);
            panner->moveCanvas(0, delta.y());
            panner->setAxisEnabled(QwtPlot::yLeft, true);
        }
        else if (y_start > canvas()->height())
        {
            // We are dragging the "bottom" axis
            panner->moveCanvas(delta.x(), 0);
        }
    }

    return true;
}


void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    /* Middle-mouse + drag is (normally) handled by the PlotPanner class.
     * However this does *not* work for dragging individual axes.
     */

    if (event->buttons() & Qt::MiddleButton)
    {
        handleMiddleMouseDrag(event);
    }

    double x = canvas_pos.x();
    double y = canvas_pos.y();

    x = invTransform(QwtPlot::xBottom, x);

    auto pen = crosshair->linePen();

    bool tracking = false;

    // Are we tracking a curve?
    if (!tracking_curve.isNull())
    {
        auto series = tracking_curve->getDataSeries();

        if (!series.isNull() && series->size() > 0)
        {
            y = tracking_curve->getDataSeries()->getValueAtTime(x);

            y = transform(tracking_curve->yAxis(), y);

            pen.setColor(tracking_curve->pen().color());
            crosshair->setLinePen(pen);

            tracking = true;
        }
    }

    if (!tracking)
    {
        pen.setColor(QColor(127, 127, 127));
        crosshair->setLinePen(pen);
    }

    double y1 = invTransform(QwtPlot::yLeft, y);
    double y2 = invTransform(QwtPlot::yRight, y);

    crosshair->setXValue(x);
    crosshair->setYValue(y1);

    emit cursorPositionChanged(x, y1, y2);

    lastMousePosition = canvas_pos;

    replot();
}


void PlotWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    lastMousePosition = canvas_pos;

    if (event->buttons() & Qt::BackButton)
    {
        zoomer->zoom(-1);
    }
    else if (event->buttons() & Qt::ForwardButton)
    {
        zoomer->zoom(+1);
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        middleMouseStartPoint = canvas_pos;

        canvas()->setCursor(Qt::SizeAllCursor);
    }
}


void PlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // Return to normal cursor
    if (canvas()->cursor() != Qt::CrossCursor)
    {
        canvas()->setCursor(Qt::CrossCursor);
    }
}


/**
 * @brief PlotWidget::mouseDoubleClickEvent callback function for mouse double click
 * @param event
 */
void PlotWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    // Middle button auto-scales graph
    if (event->button() == Qt::MiddleButton)
    {
        canvas()->setCursor(Qt::SizeAllCursor);

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

    // Left button double click performs various functions
    else if (event->button() == Qt::LeftButton)
    {
        if (canvas_pos.x() < 0)
        {
            // Double-click on left axis
            editAxisScale(QwtPlot::yLeft);
        }
        else if (canvas_pos.x() > canvas()->width())
        {
            // Double-click on right axis
            editAxisScale(QwtPlot::yRight);
        }
        else if (canvas_pos.y() > canvas()->height())
        {
            // Double-click on x axis
            editAxisScale(QwtPlot::xBottom);
        }
        else if (canvas_pos.y() > 0)
        {
            // Double-click on the "plot" area
            handlePlotDoubleClick(canvas_pos);
        }
    }
}


/**
 * @brief PlotWidget::handlePlotDoubleClick - called when the user double-clicks on the plot area
 * @param pos
 */
void PlotWidget::handlePlotDoubleClick(QPoint pos)
{
    // Map the screen position to axis coordinates
    double x = invTransform(QwtPlot::xBottom, pos.x());
    double y = 0;
    double y_left = invTransform(QwtPlot::yLeft, pos.y());
    double y_right = invTransform(QwtPlot::yRight, pos.y());

    // Work out which data series is "closest" to the point we clicked
    uint64_t index = 0;
    DataPoint dp;

    for (auto curve : curves)
    {
        if (curve.isNull() || !curve->isVisible()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull()) continue;

        y = curve->yAxis() == QwtPlot::yLeft ? y_left : y_right;

        if (series->getIndexForClosestPoint(x, y, index))
        {
            dp = series->getDataPoint(index);
            qDebug() << series->getLabel() << dp.timestamp << dp.value;
        }
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


void PlotWidget::removeAllSeries()
{
    while (curves.size() > 0)
    {
        auto curve = curves.at(0);

        if (isCurveTracked(curve))
        {
            untrackCurve();
        }

        curves.removeAt(0);
    }

    replot();
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
        autoScale(QwtPlot::xBottom, interval_bottom);
    }

    if (update_left && (axis_id == QwtPlot::yLeft || axis_id == yBoth))
    {
        autoScale(QwtPlot::yLeft, interval_left);

        // No curves available on the right axis, so update to match
        if (!update_right && axis_id == yBoth)
        {
            autoScale(QwtPlot::yRight, interval_left);
        }
    }

    if (update_right && (axis_id == QwtPlot::yRight || axis_id == yBoth))
    {
        autoScale(QwtPlot::yRight, interval_right);

        // No curves available on the left axis, so update to match
        if (!update_left && axis_id == yBoth)
        {
            autoScale(QwtPlot::yLeft, interval_right);
        }
    }

    setAutoReplot(true);
    replot();
}


/**
 * @brief PlotWidget::autoScale autoscales the specified axis to the provided interval
 * @param axisId
 * @param interval
 */
void PlotWidget::autoScale(int axisId, QwtInterval interval)
{
    double min = interval.minValue();
    double max = interval.maxValue();

    // Account for the values being "the same"
    if (min == max)
    {
        min -= 0.5f;
        max += 0.5f;
    }

    setAxisScale(axisId, min, max);
}


/**
 * @brief PlotWidget::autoScale adjusts the plot axes to fully display the DataSeries associated with the provided PlotCurve
 * @param curve
 */
void PlotWidget::autoScale(QSharedPointer<PlotCurve> curve)
{
    if (curve.isNull()) return;

    auto series = curve->getDataSeries();

    if (series.isNull() || series->size() == 0) return;

    double t_min = series->getOldestTimestamp();
    double t_max = series->getNewestTimestamp();

    double y_min = series->getMinimumValue();
    double y_max = series->getMaximumValue();

    setAxisScale(QwtPlot::xBottom, t_min, t_max);
    setAxisScale(curve->yAxis(), y_min, y_max);

    setAutoReplot(true);
    replot();
}


/**
 * @brief PlotWidget::editAxisScale manually configure scale for a given axis
 * @param axisId
 */
void PlotWidget::editAxisScale(QwtPlot::Axis axisId)
{
    auto interval = axisInterval(axisId);

    auto *dlg = new AxisScaleDialog(interval.minValue(), interval.maxValue());

    int result = dlg->exec();

    if (result == QDialog::Accepted)
    {
        setAxisScale(axisId, dlg->getMinValue(), dlg->getMaxValue());

        setAutoReplot(true);
        replot();
    }

    dlg->deleteLater();
}


int PlotWidget::getHorizontalPixels() const
{
    return width();
}
