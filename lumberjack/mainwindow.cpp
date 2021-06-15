#include <stdlib.h>
#include <qdockwidget.h>

#include <qwt_plot.h>

#include "lumberjack_version.hpp"

#include "plot_curve.hpp"
#include "data_series.hpp"
#include "plot_widget.hpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "about_dialog.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Lumberjack v" + LUMBERJACK_VERSION_STRING);

    initStatusBar();
    initSignalsSlots();


    // Add a grapho
    QDockWidget *dock = new QDockWidget("Graph", this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    PlotWidget *plot = new PlotWidget();

    dock->setWidget(plot);

    this->addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(tr("Data View"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setWidget(&dataview);

    addDockWidget(Qt::LeftDockWidgetArea, dock);

//    plot->addSeries(series_2, QwtPlot::yRight);

    // Construct some sources
    auto *manager = DataSourceManager::getInstance();

    manager->addSource(new DataSource("Source A"));
    manager->addSource(new DataSource("Source B"));
    manager->addSource(new DataSource("Source C"));
    manager->addSource(new DataSource("Source D"));

    auto src = manager->getSourceByLabel("Source B");

    src->addSeries(new DataSeries("Cat"));
    src->addSeries(new DataSeries("Dog"));
    src->addSeries(new DataSeries("Cat"));
    src->addSeries(new DataSeries("Rat"));

    auto *series = new DataSeries("My data");

    for (int idx = 0; idx < 5000; idx += 50)
    {
        for (double t = -10; t <= 10; t += 0.01)
        {
            series->addData(t + idx, (double) (rand() % 1000) - 500);
        }
    }

    src->addSeries(series);

    src = manager->getSourceByLabel("Source A");

    auto series_2 = QSharedPointer<DataSeries>(new DataSeries("Series 2"));

    for (double t = 0; t < 100; t += 0.001)
    {
        series_2->addData(t, 10 * cos(0.1 * t));
    }

    src->addSeries(series_2);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSignalsSlots()
{
    connect(ui->action_About, &QAction::triggered, this, &MainWindow::showAboutInfo);
}


void MainWindow::initStatusBar()
{
    t_pos.setText("t");
    y_pos.setText("y");

    ui->statusbar->showMessage("lumberjack");

    ui->statusbar->addPermanentWidget(&t_pos);
    ui->statusbar->addPermanentWidget(&y_pos);
}


void MainWindow::showAboutInfo()
{
    AboutDialog dlg;

    dlg.exec();
}
