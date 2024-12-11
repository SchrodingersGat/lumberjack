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
#include <qwt_text.h>

#include "axis_edit_dialog.hpp"
#include "series_editor_dialog.hpp"

#include "plot_widget.hpp"

#include "data_source_manager.hpp"
#include "lumberjack_settings.hpp"


/**
 * @brief PlotWidget::PlotWidget - Custom plot widget which provides core graphing support
 */
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

    // Setup custom graph interactions
    initZoomer();
    initPanner();
    initLegend();
    initCrosshairs();
    initGrid();

    setAcceptDrops(true);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &PlotWidget::customContextMenuRequested, this, &PlotWidget::onContextMenu);

    // Load graph widget settings

    auto *settings = LumberjackSettings::getInstance();

    QString bgColor = settings->loadSetting("graph", "defaultBackgroundColor", "#F0F0F0").toString();

    if (QColor::isValidColorName(bgColor))
    {
        setBackgroundColor(QColor(bgColor));
    }

    // Initially set an empty axis title
    setAxisTitle(QwtPlot::yLeft, " ");
    setAxisTitle(QwtPlot::yRight, " ");
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

    removeAllSeries();
    removeAllMarkers();
}


/**
 * @brief PlotWidget::getOldestTimestamp returns the oldest timestamp for visible data
 * @param ok - set to true if a value is found, else false
 * @return the "oldest" value of all displayed graph curves
 */
double PlotWidget::getOldestTimestamp(bool *ok) const
{
    double oldest = __DBL_MAX__;

    double t = 0;

    if (ok != nullptr)
    {
        *ok = false;
    }

    for (auto curve : curves)
    {
        // Ignore null curves
        if (curve.isNull()) continue;

        // Ignore hidden curves
        if (!curve->isVisible()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull()) continue;

        if (!series->hasData()) continue;

        t = series->getOldestTimestamp();

        if (t < oldest)
        {
            // A sample has been found
            if (ok != nullptr)
            {
                *ok = true;
            }

            oldest = t;
        }
    }

    return oldest;
}


/**
 * @brief PlotWidget::getNewestTimestamp returns the newest timestamp for visible data
 * @param ok - set to true if a value is found, else false
 * @return the "newest" value of all displayed graph curves
 */
double PlotWidget::getNewestTimestamp(bool *ok) const
{
    double newest = __DBL_MIN__;

    double t = 0;

    if (ok != nullptr)
    {
        *ok = false;
    }

    for (auto curve : curves)
    {
        // Ignore null curves
        if (curve.isNull()) continue;

        // Ignore hidden curves
        if (!curve->isVisible()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull()) continue;

        t = series->getNewestTimestamp();

        if (t > newest)
        {
            // A sample has been found
            if (ok != nullptr)
            {
                *ok = true;
            }

            newest = t;
        }
    }

    return newest;
}


/**
 * @brief PlotWidget::getMinimumValue returns the minimum value for visible data
 * @param ok - set to true if a value is found, else false
 * @return the "minimum" value of all displayed graph curves
 */
double PlotWidget::getMinimumValue(bool *ok) const
{
    double minimum = __DBL_MAX__;

    double value = 0;

    if (ok != nullptr)
    {
        *ok = false;
    }

    for (auto curve : curves)
    {
        // Ignore null curves
        if (curve.isNull()) continue;

        // Ignore hidden curves
        if (!curve->isVisible()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull()) continue;

        value = series->getMinimumValue();

        if (value < minimum)
        {
            // A sample has been found
            if (ok != nullptr)
            {
                *ok = true;
            }

            minimum = value;
        }
    }

    return minimum;
}


/**
 * @brief PlotWidget::getMaximumValue returns the maximum value for visible data
 * @param ok - set to true if a value is found, else false
 * @return the "maximum" value of all displayed graph curves
 */
double PlotWidget::getMaximumValue(bool *ok) const
{
    double maximum = __DBL_MIN__;

    double value = 0;

    if (ok != nullptr)
    {
        *ok = false;
    }

    for (auto curve : curves)
    {
        // Ignore null curves
        if (curve.isNull()) continue;

        // Ignore hidden curves
        if (!curve->isVisible()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull()) continue;

        value = series->getMaximumValue();

        if (value > maximum)
        {
            // A sample has been found
            if (ok != nullptr)
            {
                *ok = true;
            }

            maximum = value;
        }
    }

    return maximum;
}



/**
 * @brief PlotWidget::getVisibleCurves returns a list of visible curves
 * @param axisId is the ID of the yAxis (use yBoth for all curves)
 * @return
 */
QList<QSharedPointer<PlotCurve>> PlotWidget::getVisibleCurves(int axisId)
{
    QList<QSharedPointer<PlotCurve>> pc;

    for (auto curve : curves)
    {
        // Ignore null or hidden curves
        if (curve.isNull() || !curve->isVisible()) continue;

        if (axisId == yBoth || axisId == curve->yAxis())
        {
            pc.append(curve);
        }
    }

    return pc;
}


/**
 * @brief PlotWidget::onContextMenu - manage right-click context menu
 * @param pos - on-screen location of the right-click event
 */
void PlotWidget::onContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(pos));

    double timestamp = canvasMap(QwtPlot::xBottom).invTransform(canvas_pos.x());

    // Fit / zoom submenu
    QMenu *fitMenu = new QMenu(tr("Fit"), &menu);

    QAction *fitToScreen = fitMenu->addAction(tr("Fit all data"));
    QAction *fitX = fitMenu->addAction(tr("Fit X Axis"));
    QAction *fitLeft = fitMenu->addAction(tr("Fit Left Axis"));
    QAction *fitRight = fitMenu->addAction(tr("Fit Right Axis"));

    menu.addMenu(fitMenu);

    // Grid submenu
    QMenu *gridMenu = new QMenu(tr("Grid"), &menu);

    QAction *toggleGridX = gridMenu->addAction(tr("X Axis"));
    QAction *toggleGridY = gridMenu->addAction(tr("Y Axis"));

    toggleGridX->setCheckable(true);
    toggleGridY->setCheckable(true);

    toggleGridX->setChecked(isXGridEnabled());
    toggleGridY->setChecked(isYGridEnabled());

    menu.addMenu(gridMenu);

    // Data menu
    QMenu *dataMenu = new QMenu(tr("Data"), &menu);

    QAction *exportData = dataMenu->addAction(tr("Export Data"));
    dataMenu->addSeparator();
    QAction *imageToClipboard = dataMenu->addAction(tr("Image to Clipboard"));
    QAction *imageToFile = dataMenu->addAction(tr("Image to File"));
    dataMenu->addSeparator();
    QAction *clearAll = dataMenu->addAction(tr("Clear All"));

    exportData->setEnabled(curves.count() > 0);

    menu.addMenu(dataMenu);

    // Curve menu
    QMenu *curveMenu = menu.addMenu(tr("Tracking"));

    for (int idx = 0; idx < curves.count(); idx++)
    {
        auto curve = curves.at(idx);

        if (curve.isNull()) continue;

        auto series = curve->getDataSeries();

        if (series.isNull()) continue;

        QAction *action = new QAction(series->getLabel(), curveMenu);

        action->setCheckable(true);
        action->setChecked(isCurveTracked(curve));
        action->setData(idx);

        curveMenu->addAction(action);
    }

    QAction *untrack = new QAction(tr("Untrack"), curveMenu);

    if (curveMenu->children().count() > 0)
    {
        curveMenu->addSeparator();
        curveMenu->addAction(untrack);
        menu.addMenu(curveMenu);
    }

    // Marker submenu
    QMenu *markerMenu = new QMenu(tr("Markers"), &menu);

    QAction *addMarker = markerMenu->addAction(tr("Add Marker"));
    markerMenu->addSeparator();
    QAction *clearMarkers = markerMenu->addAction(tr("Clear Markers"));

    menu.addMenu(markerMenu);

    // Plot submenu
    QMenu *plotMenu = new QMenu(tr("Plot"), &menu);

    QAction *syncAction = plotMenu->addAction(tr("Sync Timescale"));
    syncAction->setCheckable(true);
    syncAction->setChecked(isTimescaleSynced());

    QAction *bgColor = plotMenu->addAction(tr("Set Color"));

    menu.addMenu(plotMenu);

    QAction *action = menu.exec(mapToGlobal(pos));

    // No action selection (or action cancelled)
    if (!action)
    {
        return;
    }

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
    else if (action == exportData)
    {
        exportDataToFile();
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
    else if (action->parent() == curveMenu)
    {
        if (action == untrack)
        {
            untrackCurve();
        }
        else
        {
            // List index is supplied to the action
            trackCurve(action->data().toInt());
        }
    }
    else if (action == addMarker)
    {
        this->addMarker(timestamp);
    }
    else if (action == clearMarkers)
    {
        removeAllMarkers();
    }
    else if (action == syncAction)
    {
        setTimescaleSynced(!isTimescaleSynced());
    }
    else if (action == bgColor)
    {
        selectBackgroundColor();
    }
}


/**
 * @brief PlotWidget::addMarker adds a new marker at the specified timestamp
 * @param timestamp
 */
void PlotWidget::addMarker(double timestamp)
{
    QwtPlotMarker *marker = new QwtPlotMarker();

    // TODO: Custom text
    marker->setLabel(QString("---"));

    // TODO: Custom line color, style, etc

    marker->setValue(timestamp, 0);
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setLabelOrientation(Qt::Vertical);
    marker->setLabelAlignment(Qt::AlignBottom | Qt::AlignRight);

    marker->attach(this);

    markers.append(marker);

    replot();
}


/**
 * @brief PlotWidget::removeAllMarkers removes all displayed markers
 */
void PlotWidget::removeAllMarkers()
{
    for (int idx = 0; idx < markers.count(); idx++)
    {
        QwtPlotMarker* marker = markers.at(idx);

        if (!marker) continue;

        marker->detach();
        delete marker;
    }

    markers.clear();

    replot();
}


/**
 * @brief PlotWidget::setBackgroundColor launches a dialog to select the background color
 */
void PlotWidget::selectBackgroundColor()
{
    QColor c = QColorDialog::getColor(this->canvasBackground().color().rgb());

    c.setAlpha(255);

    setBackgroundColor(c);

    auto *settings = LumberjackSettings::getInstance();

    // Save the background color as 'default'
    settings->saveSetting("graph", "defaultBackgroundColor", c.name());
}


/**
 * @brief PlotWidget::exportDataToFile - export data to a file
 */
void PlotWidget::exportDataToFile()
{
    auto manager = DataSourceManager::getInstance();

    QList<DataSeriesPointer> series;

    for (auto curve : curves)
    {
        series.append(curve->getDataSeries());
    }

    manager->saveToFile(series);
}


/**
 * @brief PlotWidget::saveImageToClipboard - copy the current plot view to clipboard
 */
void PlotWidget::saveImageToClipboard()
{
    auto *cliboard = QGuiApplication::clipboard();

    cliboard->setPixmap(grab());
}


/**
 * @brief PlotWidget::saveImageToFile - Save the current plot view to an image file
 */
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



void PlotWidget::setTimeInterval(const QwtInterval &interval)
{
    autoScale(QwtPlot::xBottom, interval);

    replot();
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
    zoomer->setRubberBandPen(pen);

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
    Q_UNUSED(event)
}


/**
 * @brief PlotWidget::dropEvent is called when an accepted drag event is dropped on the widget
 * @param event
 */
void PlotWidget::dropEvent(QDropEvent *event)
{
    auto *mime = event->mimeData();
    auto *manager = DataSourceManager::getInstance();

    if (!mime || !manager)
    {
        return;
    }

    // DataSeries is dropped onto this PlotWidget
    if (mime->hasFormat("source") && mime->hasFormat("series"))
    {
        QString source_lbl = mime->data("source");
        QString series_lbl = mime->data("series");

        auto series = manager->findSeries(source_lbl, series_lbl);

        if (series.isNull())
        {
            qCritical() << "Could not find graph matching" << source_lbl << ":" << series_lbl;
            return;
        }

        addSeries(series, QwtPlot::yLeft);

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


/**
 * @brief PlotWidget::initLegend - initialize legend
 */
void PlotWidget::initLegend()
{
    // Initialize left axis legend
    leftLegend = new PlotLegend(this);
    leftLegend->attach(this);
    leftLegend->setAxisAlignment(QwtPlot::yLeft);

    // Initialize right axis legend
    rightLegend = new PlotLegend(this);
    rightLegend->attach(this);
    rightLegend->setAxisAlignment(QwtPlot::yRight);
}


/**
 * @brief PlotWidget::initGrid - initialize plot grid
 */
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

    // Signal to other widgets that we have updated the view
    updateCurrentView();
}


void PlotWidget::updateCurrentView()
{
    if (isTimescaleSynced())
    {
        emit viewChanged(axisInterval(QwtPlot::xBottom));
    }
}


void PlotWidget::updateTimestampLimits()
{
    if (isTimescaleSynced())
    {
        emit timestampLimitsChanged(QwtInterval(getOldestTimestamp(), getNewestTimestamp()));
    }
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


/**
 * @brief PlotWidget::legendClicked - Callback when a legend item is clicked
 * @param item
 */
void PlotWidget::legendClicked(const QwtPlotItem *item)
{
    if (!item) return;
    auto modifiers = QApplication::keyboardModifiers();

    for (auto curve : curves)
    {
        if (!curve.isNull() && (QwtPlotItem*) &(*curve) == item)
        {
            auto series = curve->getDataSeries();

            if (series.isNull()) break;

            if (modifiers == Qt::ShiftModifier)
            {
                // Toggle curve visibility
                curve->setVisible(!curve->isVisible());

                updateTimestampLimits();
            }
            else if (modifiers == Qt::ControlModifier)
            {
                // Remove the data series
                removeSeries(series);
            }
            else if (modifiers == Qt::AltModifier)
            {
                // Switch curve to the other axis
                curve->detach();
                curve->setYAxis(curve->yAxis() == QwtPlot::yLeft ? QwtPlot::yRight : QwtPlot::yLeft);
                curve->attach(this);
            }
            else
            {
                trackCurve(curve);
            }

            // Exit once we have processed the matching item
            break;
        }
    }

    setAutoReplot(false);
    replot();
}


/**
 * @brief PlotWidget::legendDoubleClicked - Callback when a legend item is double clicked
 * @param item
 */
void PlotWidget::legendDoubleClicked(const QwtPlotItem *item)
{
    if (!item) return;

    SeriesEditorDialog *dlg = nullptr;

    for (auto curve : curves)
    {
        if (!curve.isNull() && (QwtPlotItem*) &(*curve) == item)
        {
            auto series = curve->getDataSeries();

            if (series.isNull()) break;

            dlg = new SeriesEditorDialog(series);

            // Exit once we have processed the matching item
            break;
        }
    }

    if (dlg)
    {
        dlg->exec();
        replot();
        dlg->deleteLater();
    }
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
 * @brief PlotWidget::trackCurve - tracks the curve at the specified index
 * @param index
 */
void PlotWidget::trackCurve(int index)
{
    if (index < 0 || index >= curves.count()) return;

    trackCurve(curves.at(index));
}


/**
 * @brief PlotWidget::untrackCurve removes curve tracking association for this widget
 */
void PlotWidget::untrackCurve()
{
    tracking_curve = QSharedPointer<PlotCurve>(nullptr);
}


/**
 * @brief PlotWidget::eventFilter
 * @param target
 * @param event
 * @return
 */
bool PlotWidget::eventFilter(QObject *target, QEvent *event)
{
    if (target && event)
    {
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
        {
            // Hand the event off to each legend
            const QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (mouseEvent)
            {
                if (leftLegend->handleMousePressEvent(mouseEvent)) return true;
                if (rightLegend->handleMousePressEvent(mouseEvent)) return true;
            }

            break;
        }
        default:
            break;
        }

    }

    return QwtPlot::eventFilter(target, event);
}


/**
 * @brief PlotWidget::wheelEvent - Respond to a mouse wheel event
 * @param event
 */
void PlotWidget::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();

    // Ignore null events
    if (delta.isNull() || delta.y() == 0) return;

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

    if (delta.y() > 0)
    {
        factor = 1 / factor;
    }

    // Map the mouse position to the underlying canvas,
    // otherwise the size of the displayed axes causes offset issues
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->position().toPoint()));

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

    updateCursorShape();

    replot();
}


/**
 * @brief PlotWidget::handleMiddleMouseDrag - handle a mouse drag event with middle mouse button pressed
 * @param event
 */
void PlotWidget::handleMiddleMouseDrag(QMouseEvent *event)
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
}


void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    updateCursorShape(event);

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

    if (!crosshair)
    {
        initCrosshairs();
    }

    auto pen = crosshair->linePen();

    bool tracking = false;

    // Are we tracking a curve?
    if (isCurveTrackingEnabled() && !tracking_curve.isNull())
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
    }

    updateCursorShape(event);
}


void PlotWidget::mouseReleaseEvent(QMouseEvent *event)
{
    updateCursorShape(event);
}


/**
 * @brief PlotWidget::mouseDoubleClickEvent callback function for mouse double click
 * @param event
 */
void PlotWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint canvas_pos = canvas()->mapFromGlobal(mapToGlobal(event->pos()));

    updateCursorShape(event);

    // Middle button auto-scales graph
    if (event->button() == Qt::MiddleButton)
    {
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
        // Double-click on left axis
        if (canvas_pos.x() < 0)
        {
            editAxisScale(QwtPlot::yLeft);
        }
        // Double-click on right axis
        else if (canvas_pos.x() > canvas()->width())
        {
            editAxisScale(QwtPlot::yRight);
        }
        // Double-click on x axis
        else if (canvas_pos.y() > canvas()->height())
        {
            editAxisScale(QwtPlot::xBottom);
        }
    }
}


/**
 * @brief PlotWidget::updateCursorShape updates the mouse cursor shape based on user intent
 *
 * - Shift modifier = horizontal adjust
 * - Control modifier = vertical adjust
 * - Middle mouse press = pan
 */
void PlotWidget::updateCursorShape(QMouseEvent *event)
{
    // Default shape unless otherwise modified
    Qt::CursorShape shape = Qt::CrossCursor;

    auto modifiers = QApplication::keyboardModifiers();

    if (event && (event->buttons() & Qt::MiddleButton))
    {
        shape = Qt::SizeAllCursor;
    }

    else
    {
        if (modifiers == Qt::ShiftModifier)
        {
            shape = Qt::SizeHorCursor;
        }
        else if (modifiers == Qt::ControlModifier)
        {
            shape = Qt::SizeVerCursor;
        }
    }

    // Update cursor shape if it is different
    if (canvas()->cursor() != shape)
    {
        canvas()->setCursor(shape);
    }
}


/**
 * @brief PlotWidget::addSeries - adds the provided DataSeries to the plot
 * @param series - pointer to the DataSeries
 * @param axis_id - axis ID (either QwtPlot::yLeft or QwtPlot::yRight)
 * @return true if the series was added
 */
bool PlotWidget::addSeries(DataSeriesPointer series, int axis_id)
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
    PlotCurveUpdater* worker = generateNewWorker(series);
    PlotCurve *curve = new PlotCurve(series, worker);

    curve->setYAxis(axis_id);
    curve->attach(this);

    curves.push_back(QSharedPointer<PlotCurve>(curve));

    auto interval = axisInterval(QwtPlot::xBottom);

    // Perform initial data sampling
    curve->resampleData(interval.minValue(), interval.maxValue(), getHorizontalPixels());

    setAutoReplot(true);
    replot();

    updateTimestampLimits();

    return true;
}


/**
 * @brief PlotWidget::removeSeries - remove the curve associated with the provided DataSeries
 * @param series - DataSeries pointer
 * @return true if the DataSeries existed and was removed
 */
bool PlotWidget::removeSeries(DataSeriesPointer series)
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

            updateTimestampLimits();

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


/**
 * @brief PlotWidget::removeAllSeries removes all curves from the plot
 */
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


/**
 * @brief PlotWidget::autoScale - autoscale the selected axis
 * @param axis_id is the ID of the axis (e.g. QwtPlot::yLeft)
 */
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

    QwtText title = axisTitle(axisId);

    // TODO: Allow customization of color and size of axis title
    auto font = title.font();
    font.setPointSize(8);
    title.setFont(font);

    auto *dlg = new AxisEditorDialog(
                axisId,
                title.text(),
                interval.minValue(),
                interval.maxValue());

    int result = dlg->exec();

    if (result == QDialog::Accepted)
    {
        setAxisScale(axisId, dlg->getMinValue(), dlg->getMaxValue());

        title.setText(dlg->getTitle());

        setAxisTitle(axisId, title);

        setAutoReplot(true);
        replot();
    }

    dlg->deleteLater();
}


/**
 * @brief PlotWidget::getHorizontalPixels returns the number of horizontal dispay pixels
 */
int PlotWidget::getHorizontalPixels() const
{
    return width();
}


/**
 * @brief PlotWidget::generateNewWorker - Create a new curve sampling worker
 * @return
 */
PlotCurveUpdater* PlotWidget::generateNewWorker(DataSeriesPointer series)
{
    return new PlotCurveUpdater(*series);
}
