#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlabel.h>

#include "data_series.hpp"
#include "plot_curve.hpp"

#include "debug_widget.hpp"
#include "plot_widget.hpp"
#include "stats_widget.hpp"
#include "dataview_widget.hpp"
#include "timeline_widget.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadDummyData(void);

public slots:

    void onTimescaleChanged(const QwtInterval &view);
    void updateCursorPos(double t, double y1, double y2);

    void loadDroppedFile(QString filename);

protected:
    void initMenus(void);
    void initDocks(void);
    void initSignalsSlots(void);
    void initStatusBar(void);

    void importData(void);

    void loadWorkspaceSettings(void);
    void saveWorkspaceSettings(void);

    QList<PlotWidget*> plots();

    void closeEvent(QCloseEvent *event);

protected slots:

    void showAboutInfo(void);

    void toggleDebugView(void);
    void toggleDataView(void);
    void toggleTimelineView(void);
    void toggleStatisticsView(void);

    void hideDockedWidget(QWidget *widget);

    void seriesRemoved(QSharedPointer<DataSeries> series);

private:
    Ui::MainWindow *ui;

    QLabel t_pos;
    QLabel y1_pos;
    QLabel y2_pos;

    PlotWidget plotView;
    DataviewWidget dataView;
    StatsWidget statsView;
    TimelineWidget timelineView;

    DebugWidget debugWidget;

};
#endif // MAINWINDOW_H
