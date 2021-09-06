#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlabel.h>

#include "data_series.hpp"
#include "plot_curve.hpp"

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

public slots:

    void updateCursorPos(double t, double y1, double y2);

protected:
    void initMenus(void);
    void initDocks(void);
    void initSignalsSlots(void);
    void initStatusBar(void);

    void importData(void);

protected slots:
    void showAboutInfo(void);

    void toggleDataView(void);
    void toggleTimelineView(void);
    void toggleStatisticsView(void);

    void hideDockedWidget(QWidget *widget);

private:
    Ui::MainWindow *ui;

    QLabel t_pos;
    QLabel y1_pos;
    QLabel y2_pos;

    PlotWidget plotView;
    DataviewWidget dataView;
    StatsWidget statsView;
    TimelineWidget timelineView;

};
#endif // MAINWINDOW_H
