#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlabel.h>

#include "data_series.hpp"

#include "debug_widget.hpp"
#include "plot_widget.hpp"
#include "fft_widget.hpp"
#include "stats_widget.hpp"
#include "dataview_widget.hpp"
#include "timeline_widget.hpp"
#include "plugin_registry.hpp"

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
    void loadDataFromFile(QString filename);

protected:
    void initMenus(void);
    void initDocks(void);
    void initSignalsSlots(void);
    void initStatusBar(void);

    QList<QSharedPointer<FileDataSource>> getFileImporters();

    void loadWorkspaceSettings(void);
    void saveWorkspaceSettings(void);

    void closeEvent(QCloseEvent *event);

protected slots:

    void showAboutInfo(void);

    void importData(void);
    void loadDroppedFile(QString filename);

    void toggleDebugView(void);
    void toggleDataView(void);
    void toggleFftView(void);
    void toggleTimelineView(void);
    void toggleStatisticsView(void);

    void addPlot();
    void removePlot(QSharedPointer<PlotWidget> plot);

    void hideDockedWidget(QWidget *widget);

    void seriesRemoved(QSharedPointer<DataSeries> series);

private:
    Ui::MainWindow *ui;

    QList<QSharedPointer<PlotWidget>> plots;

    QLabel t_pos;
    QLabel y1_pos;
    QLabel y2_pos;

    DataviewWidget dataView;
    StatsWidget statsView;
    TimelineWidget timelineView;
    FFTWidget fftView;

    DebugWidget debugWidget;

    Lumberjack::PluginRegistry pluginRegistry;

};
#endif // MAINWINDOW_H
