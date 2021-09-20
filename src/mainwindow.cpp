#include <stdlib.h>
#include <qdockwidget.h>
#include <math.h>

#include <qfiledialog.h>

#include <qwt_plot.h>

#include "lumberjack_version.hpp"

#include "helpers.hpp"
#include "lumberjack_settings.hpp"

#include "plot_curve.hpp"
#include "data_series.hpp"
#include "plot_widget.hpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "about_dialog.hpp"

// TODO: In the future, replace these explicit includes with a "plugin" architecture
#include "csv_importer.hpp"
#include "cedat_importer.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Lumberjack v" + LUMBERJACK_VERSION_STRING);

    initMenus();
    initDocks();
    initStatusBar();
    initSignalsSlots();

    plotView.setParent(this);
    setCentralWidget(&plotView);

    setAcceptDrops(true);

    // Construct some sources
    auto *manager = DataSourceManager::getInstance();

    /*
    manager->addSource(new DataSource("Source A"));
    manager->addSource(new DataSource("Source B"));
    manager->addSource(new DataSource("Source C"));
    manager->addSource(new DataSource("Source D"));

    auto src = manager->getSourceByLabel("Source B");

    src->addSeries(new DataSeries("Cat"));
    src->addSeries(new DataSeries("Dog"));
    src->addSeries(new DataSeries("Rat"));

    auto *series = new DataSeries("My data");

    for (int idx = 0; idx < 5000; idx += 50)
    {
        for (double t = -10; t <= 10; t += 0.01)
        {
            series->addData(t + idx, (double) (rand() % 1000) - 500);
        }

        src->getSeriesByLabel("Cat")->addData(idx + rand() % 20 - 10, (double) (rand() % 500) - 250);
        src->getSeriesByLabel("Rat")->addData(idx + rand() % 20 - 10, (double) (rand() % 500) - 250);
        src->getSeriesByLabel("Dog")->addData(idx + rand() % 20 - 10, (double) (rand() % 500) - 250);
    }

    src->addSeries(series);

    src = manager->getSourceByLabel("Source A");

    auto series_2 = QSharedPointer<DataSeries>(new DataSeries("Series 2"));
    auto series_3 = QSharedPointer<DataSeries>(new DataSeries("Series 3"));

    for (double t = 0; t < 100; t += 0.0001)
    {
        series_2->addData(t, 10 * cos(1 * t));
        series_3->addData(t, 10 * sin(15 * t));
    }

    src->addSeries(series_2);
    src->addSeries(series_3);

    */
}


MainWindow::~MainWindow()
{
    qDebug() << "~MainWindow()";

    DataSourceManager::cleanup();
    LumberjackSettings::cleanup();

    delete ui;
}


/**
 * @brief MainWindow::initMenus initializes menus and menu actions
 */
void MainWindow::initMenus()
{
    connect(ui->actionE_xit, &QAction::triggered, this, &QMainWindow::close);

    connect(ui->action_About, &QAction::triggered, this, &MainWindow::showAboutInfo);

    connect(ui->action_Data_View, &QAction::triggered, this, &MainWindow::toggleDataView);

    connect(ui->action_Timeline, &QAction::triggered, this, &MainWindow::toggleTimelineView);

    connect(ui->action_Statistics, &QAction::triggered, this, &MainWindow::toggleStatisticsView);

    connect(ui->action_Import_Data, &QAction::triggered, this, &MainWindow::importData);
}


/**
 * @brief MainWindow::initDocks initializes the various dockable widgets
 */
void MainWindow::initDocks()
{
    this->setDockOptions(AnimatedDocks | AllowNestedDocks);

    toggleDataView();
    toggleTimelineView();
//    toggleStatisticsView();
}


void MainWindow::initSignalsSlots()
{
    // TODO

    // Plot widget signals
    connect(&plotView, &PlotWidget::cursorPositionChanged, this, &MainWindow::updateCursorPos);
    connect(&plotView, &PlotWidget::viewChanged, this, &MainWindow::onTimescaleChanged);
    connect(&plotView, &PlotWidget::viewChanged, &timelineView, &TimelineWidget::updateViewLimits);
    connect(&plotView, &PlotWidget::timestampLimitsChanged, &timelineView, &TimelineWidget::updateTimeLimits);

    // File drops
    connect(&plotView, &PlotWidget::fileDropped, this, &MainWindow::loadDroppedFile);
    connect(&dataView, &DataviewWidget::fileDropped, this, &MainWindow::loadDroppedFile);


    // Timeline view
    connect(&timelineView, &TimelineWidget::timeUpdated, &plotView, &PlotWidget::setTimeInterval);

}


void MainWindow::initStatusBar()
{
    ui->statusbar->showMessage("lumberjack");

    ui->statusbar->addPermanentWidget(&t_pos);
    ui->statusbar->addPermanentWidget(&y1_pos);
    ui->statusbar->addPermanentWidget(&y2_pos);

    updateCursorPos(0, 0, 0);
}


void MainWindow::onTimescaleChanged(const QwtInterval &viewRect)
{
    // TODO
}


/**
 * @brief MainWindow::updateCursorPos - callback function when the cursor position changes
 * @param t
 * @param y1
 * @param y2
 */
void MainWindow::updateCursorPos(double t, double y1, double y2)
{
    t_pos.setText("t: " + fixedWidthNumber(t));
    y1_pos.setText("y1: " + fixedWidthNumber(y1));
    y2_pos.setText("y2: " + fixedWidthNumber(y2));
}


void MainWindow::showAboutInfo()
{
    AboutDialog dlg;

    dlg.exec();
}


void MainWindow::loadDroppedFile(QString filename)
{
    // Determine which loader plugin to use

    // Get list of available importers

    // TODO: In the future, push this off to a "plugin" module!!

    QList<QSharedPointer<FileDataSource>> sources;

    sources.append(QSharedPointer<FileDataSource>(new CSVImporter()));
    sources.append(QSharedPointer<FileDataSource>(new CEDATImporter()));

    // For now, use the first available plugin
    // TODO: In the future, handle multiple plugin matches

    QFileInfo info(filename);

    QString extension = info.suffix();

    // TODO: Display errors
    QStringList errors;

    for (auto source : sources)
    {
        if (source->supportsFileType(extension))
        {
            bool result = source->loadData(filename, errors);

            if (result)
            {
                DataSourceManager::getInstance()->addSource(source);
            }
        }
    }
}


void MainWindow::importData()
{
    // Get list of available importers

    // TODO: In the future, push this off to a "plugin" module!!

    QList<QSharedPointer<FileDataSource>> sources;

    sources.append(QSharedPointer<FileDataSource>(new CSVImporter()));
    sources.append(QSharedPointer<FileDataSource>(new CEDATImporter()));

    // Assemble list of available files

    // Assemble set of supported file types
    QStringList supportedFileTypes;

    QStringList filePatterns;

    for (auto source : sources)
    {
        if (source.isNull()) continue;

        for (QString fileType : source->getSupportedFileTypes())
        {
            if (!supportedFileTypes.contains(fileType))
            {
                supportedFileTypes.append(fileType);
            }
        }

        filePatterns.append(source->getFilePattern());
    }

    // Load a file
    QFileDialog dialog(this);

    dialog.setWindowTitle(tr("Import Data from File"));

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(filePatterns.join(";;"));
    dialog.setViewMode(QFileDialog::Detail);

    int result = dialog.exec();

    if (result != QDialog::Accepted)
    {
        // User cancelled the import process
        return;
    }

    // Determine which plugin loaded the data
    QString filter = dialog.selectedNameFilter();
    QStringList files = dialog.selectedFiles();

    if (filter.isEmpty() || files.length() != 1)
    {
        return;
    }

    QString filename = files.first();

    for (auto source : sources)
    {
        if (!source.isNull() && source->getFilePattern() == filter)
        {
            // TODO: Support cases where multiple plugins may match the same file extension

            QStringList errors;

            bool result = source->loadData(filename, errors);

            // Add the loaded data to the DataSourceManager
            if (result)
            {
                DataSourceManager::getInstance()->addSource(source);
            }
        }
    }
}


void MainWindow::hideDockedWidget(QWidget *widget)
{
    for (auto *dock : findChildren<QDockWidget*>())
    {
        if (dock->widget() == widget)
        {
            dock->close();
            return;
        }
    }
}


/**
 * @brief MainWindow::toggleDataView toggles visibility of the "data view" dock
 */
void MainWindow::toggleDataView(void)
{
    if (dataView.isVisible())
    {
        hideDockedWidget(&dataView);
    }
    else
    {
        QDockWidget* dock = new QDockWidget(tr("Data View"), this);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(&dataView);

        addDockWidget(Qt::LeftDockWidgetArea, dock);
    }

    ui->action_Data_View->setCheckable(true);
    ui->action_Data_View->setChecked(dataView.isVisible());
}


/**
 * @brief MainWindow::toggleTimelineView toggles visibility of the "timeline" dock
 */
void MainWindow::toggleTimelineView(void)
{
    if (timelineView.isVisible())
    {
        hideDockedWidget(&timelineView);
    }
    else
    {
        QDockWidget* dock = new QDockWidget(tr("Timeline"), this);
        dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        dock->setWidget(&timelineView);

        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }

    ui->action_Timeline->setCheckable(true);
    ui->action_Timeline->setChecked(timelineView.isVisible());
}


/**
 * @brief MainWindow::toggleStatisticsView toggles visibility of the "statistics" dock
 */
void MainWindow::toggleStatisticsView(void)
{
    if (statsView.isVisible())
    {
        hideDockedWidget(&statsView);
    }
    else
    {
        QDockWidget *dock = new QDockWidget(tr("Stats View"), this);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(&statsView);

        addDockWidget(Qt::LeftDockWidgetArea, dock);
    }

    ui->action_Statistics->setCheckable(true);
    ui->action_Statistics->setCheckable(statsView.isVisible());
}
