#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "plot_curve.hpp"
#include "data_series.hpp"
#include "plot_widget.hpp"

#include <qdockwidget.h>

#include <stdlib.h>
#include <qwt_plot.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initStatusBar();
    initSignalsSlots();


    // Add a grapho
    QDockWidget *dock = new QDockWidget("Graph", this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    PlotWidget *plot = new PlotWidget();

    dock->setWidget(plot);

    this->addDockWidget(Qt::LeftDockWidgetArea, dock);

    // Construct a new series
    series = QSharedPointer<DataSeries>(new DataSeries("My Series"));

    for (int idx = 0; idx < 5000; idx += 50)
    {
        for (double t = -10; t <= 10; t += 0.01)
        {
            series->addData(t + idx, (double) (rand() % 1000) - 500);
        }
    }

    plot->addSeries(series);
    plot->addSeries(series);
    plot->addSeries(series);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSignalsSlots()
{

}


void MainWindow::initStatusBar()
{
    t_pos.setText("t");
    y_pos.setText("y");

    ui->statusbar->showMessage("lumberjack");

    ui->statusbar->addPermanentWidget(&t_pos);
    ui->statusbar->addPermanentWidget(&y_pos);
}
