#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlabel.h>

#include "data_series.hpp"
#include "plot_curve.hpp"

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

protected:
    void initSignalsSlots(void);
    void initStatusBar(void);

private:
    Ui::MainWindow *ui;

    QLabel t_pos;
    QLabel y_pos;

    QSharedPointer<DataSeries> series;

};
#endif // MAINWINDOW_H
