#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include <QMouseEvent>
#include <QWheelEvent>

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_legend.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>

#include "plot_panner.hpp"
#include "plot_curve.hpp"


class PlotWidget : public QwtPlot
{
    Q_OBJECT

public:
    PlotWidget();
    virtual ~PlotWidget();

    virtual void updateLayout(void) override;

    double getOldestTimestamp(bool *ok = nullptr) const;
    double getNewestTimestamp(bool *ok = nullptr) const;

    double getMinimumValue(bool *ok = nullptr) const;
    double getMaximumValue(bool *ok = nullptr) const;

    QList<QSharedPointer<PlotCurve>> getVisibleCurves(int axisId = yBoth);

    bool isTimescaleSynced(void) const { return syncedTimescale; }
    void setTimescaleSynced(bool sync) { syncedTimescale = sync; }

signals:
    // Emitted whenever the view rect is changed
    void viewChanged(const QwtInterval &view);

    // Emitted whenever the timestamp limits are changed
    void timestampLimitsChanged(const QwtInterval &limits);

    void fileDropped(QString filename);

    void cursorPositionChanged(double &t, double &y1, double &y2);

public slots:
    int getHorizontalPixels(void) const;

    bool addSeries(QSharedPointer<DataSeries> series, int axis_id = QwtPlot::yLeft);
    bool addSeries(DataSeries *series) { return addSeries(QSharedPointer<DataSeries>(series)); }

    bool removeSeries(QSharedPointer<DataSeries> series);
    bool removeSeries(DataSeries *series) { return removeSeries(QSharedPointer<DataSeries>(series)); }
    bool removeSeries(QString label);
    void removeAllSeries();

    void addMarker(double timestamp);
    void removeAllMarkers();

    void autoScale(int axis_id = yBoth);
    void autoScale(int axis_id, QwtInterval interval);
    void autoScale(QSharedPointer<PlotCurve> curve);

    void setBackgroundColor(QColor color);

    void onContextMenu(const QPoint &pos);

    bool isXGridEnabled() { return grid->xEnabled(); }
    bool isYGridEnabled() { return grid->yEnabled(); }

    void xGridEnable(bool en) { grid->enableX(en); }
    void yGridEnable(bool en) { grid->enableY(en); }

    void selectBackgroundColor();

    void saveImageToClipboard();
    void saveImageToFile();

    void setTimeInterval(const QwtInterval &interval);

protected slots:

    void editAxisScale(QwtPlot::Axis axisId);
    void legendClicked(const QVariant &item_info, int index);

protected:
    // Mouse actions
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    // Drag-n-drop actions
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    void handleMiddleMouseDrag(QMouseEvent *event);

    void initZoomer(void);
    void initPanner(void);
    void initLegend(void);
    void initCrosshairs(void);
    void initGrid(void);

    virtual PlotCurveUpdater* generateNewWorker(QSharedPointer<DataSeries> series);

    void resampleCurves(int axis_id = yBoth);

    void updateCursorShape(QMouseEvent *event = nullptr);

    void updateCurrentView();
    void updateTimestampLimits();

    // Curve tracking
    virtual bool isCurveTrackingEnabled(void) const { return true; }
    bool isCurveTracked(void);
    bool isCurveTracked(QSharedPointer<PlotCurve> curve);
    void trackCurve(QSharedPointer<PlotCurve> curve);
    void trackCurve(int index);
    void untrackCurve(void);

    QwtPlotZoomer *zoomer;
    PlotPanner *panner;
    QwtLegend *legend;
    QwtPlotGrid *grid;

    QwtPlotMarker *crosshair;
    QwtPlotMarker *curveTracker;

    // List of curves attached to this widget
    QList<QSharedPointer<PlotCurve>> curves;

    // List of markers attached to this widget
    QList<QwtPlotMarker*> markers;

    QSharedPointer<PlotCurve> tracking_curve;

    // Specify "both" y axes
    static const int yBoth = -1;

    // Starting point of middle-mouse drag, in canvas coordinates
    QPoint middleMouseStartPoint;

    // Most recent mouse position, in canvas coordinates
    QPoint lastMousePosition;

    // Is this graph synced to the global timescale?
    bool syncedTimescale = true;
};

#endif // PLOT_WIDGET_HPP
