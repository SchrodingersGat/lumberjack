#include <stdlib.h>
#include <qdockwidget.h>
#include <math.h>

#include <qfiledialog.h>
#include <qsharedpointer.h>

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
#include "mavlink_importer.hpp"


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

    // Construct a central layout for displaying plot widgets
    QWidget *central = new QWidget();
    central->setLayout(&plotLayout);
    setCentralWidget(central);

    // Add an initial plot
    addPlot();

    setAcceptDrops(true);

    loadWorkspaceSettings();
}


MainWindow::~MainWindow()
{
    qDebug() << "~MainWindow()";

    DataSourceManager::cleanup();
    LumberjackSettings::cleanup();

    delete ui;
}


/*
 * Load a set of dummy data for testing
 */
void MainWindow::loadDummyData()
{

    // Construct some sources
    auto *manager = DataSourceManager::getInstance();

    manager->addSource(new DataSource("Source A"));
    manager->addSource(new DataSource("Source B"));

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
    auto series_4 = QSharedPointer<DataSeries>(new DataSeries("Series 4"));

    for (double t = 0; t < 100; t += 0.0001)
    {
        series_2->addData(t, 10 * cos(1 * t));
        series_3->addData(t, 10 * sin(15 * t));

        // Random time data wobble
        double t_offset = (double) (rand() % 1000 - 500) / 1000;
        t_offset *= 0.0001;

        series_4->addData(t + t_offset, 15 * cos(5 * t));
    }

    src->addSeries(series_2);
    src->addSeries(series_3);
    src->addSeries(series_4);
}


/*
 * Load workspace settings
 */
void MainWindow::loadWorkspaceSettings()
{
    auto *settings = LumberjackSettings::getInstance();

    QByteArray state = settings->loadSetting("mainwindow", "state", QByteArray()).toByteArray();
    QByteArray geometry = settings->loadSetting("mainwindow", "geometry", QByteArray()).toByteArray();

    restoreState(state);
    restoreGeometry(geometry);
}


/*
 * Save workspace settings
 */
void MainWindow::saveWorkspaceSettings()
{
    auto *settings = LumberjackSettings::getInstance();

    settings->saveSetting("mainwindow", "showDataView", dataView.isVisible());
    settings->saveSetting("mainwindow", "showTimelineView", timelineView.isVisible());
    settings->saveSetting("mainwindow", "showStatsView", statsView.isVisible());
    settings->saveSetting("mainwindow", "showDebugView", debugWidget.isVisible());

    settings->saveSetting("mainwindow", "state", saveState());
    settings->saveSetting("mainwindow", "geometry", saveGeometry());
}



/**
 * @brief MainWindow::initMenus initializes menus and menu actions
 */
void MainWindow::initMenus()
{
    // File menu
    connect(ui->action_Import_Data, &QAction::triggered, this, &MainWindow::importData);
    connect(ui->actionE_xit, &QAction::triggered, this, &QMainWindow::close);

    // View menu
    connect(ui->action_Data_View, &QAction::triggered, this, &MainWindow::toggleDataView);
    connect(ui->action_Timeline, &QAction::triggered, this, &MainWindow::toggleTimelineView);
    connect(ui->action_Statistics, &QAction::triggered, this, &MainWindow::toggleStatisticsView);
    connect(ui->action_FFT, &QAction::triggered, this, &MainWindow::toggleFftView);

    // Graphs menu
    connect(ui->action_Add_Graph, &QAction::triggered, this, &MainWindow::addPlot);

    // Help menu
    connect(ui->action_About, &QAction::triggered, this, &MainWindow::showAboutInfo);
    connect(ui->action_Debug, &QAction::triggered, this, &MainWindow::toggleDebugView);
}


/**
 * @brief MainWindow::initDocks initializes the various dockable widgets
 */
void MainWindow::initDocks()
{
    this->setDockOptions(AnimatedDocks | AllowNestedDocks);

    auto *settings = LumberjackSettings::getInstance();

    if (settings->loadBoolean("mainwindow", "showStatsView"))
    {
        toggleStatisticsView();
    }

    if (settings->loadBoolean("mainwindow", "showDataView"))
    {
        toggleDataView();
    }

    if (settings->loadBoolean("mainwindow", "showTimelineView"))
    {
        toggleTimelineView();
    }

    if (settings->loadBoolean("mainwindow", "showDebugView"))
    {
        toggleDebugView();
    }
}


/*
 * Configure SIGNAL/SLOT connections
 */
void MainWindow::initSignalsSlots()
{
    DataViewTree *tree = dataView.getTree();

    if (tree)
    {
        connect(tree, &DataViewTree::onSeriesRemoved, this, &MainWindow::seriesRemoved);
    }

    connect(&dataView, &DataviewWidget::fileDropped, this, &MainWindow::loadDroppedFile);

    // Timeline view
    connect(&timelineView, &TimelineWidget::timeUpdated, this, &MainWindow::onTimescaleChanged);

}


/*
 * Configure the status bar at the bottom of the window
 */
void MainWindow::initStatusBar()
{
    ui->statusbar->showMessage("lumberjack");

    ui->statusbar->addPermanentWidget(&t_pos);
    ui->statusbar->addPermanentWidget(&y1_pos);
    ui->statusbar->addPermanentWidget(&y2_pos);

    updateCursorPos(0, 0, 0);
}


/*
 * Callback when the timescale is adjusted.
 */
void MainWindow::onTimescaleChanged(const QwtInterval &viewInterval)
{
    auto source = sender();

    QList<QSharedPointer<DataSeries>> seriesList;

    // Update the timescale on other plots
    for (auto plot : plots)
    {
        // Ignore null pointers
        if (plot.isNull()) continue;

        // Ignore plots which are not synced
        if (!plot->isTimescaleSynced()) continue;

        auto curves = plot->getVisibleCurves();

        for (auto curve : curves)
        {
            auto series = curve->getDataSeries();

            if (series.isNull()) continue;

            seriesList.append(series);
        }

        // Prevent updating of same plot
        if (plot == source) continue;

        plot->setTimeInterval(viewInterval);
    }

    // Update the "statistics" view
    statsView.updateStats(seriesList, viewInterval);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    saveWorkspaceSettings();
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


/*
 * Display the "About" dialog
 */
void MainWindow::showAboutInfo()
{
    AboutDialog dlg;

    dlg.exec();
}


/*
 * Toggle display of the "Debug" window
 */
void MainWindow::toggleDebugView()
{
    ui->action_Debug->setCheckable(true);

    if (debugWidget.isVisible())
    {
        hideDockedWidget(&debugWidget);
        ui->action_Debug->setChecked(false);
    }
    else
    {
        QDockWidget *dock = new QDockWidget(tr("Debug View"), this);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(&debugWidget);

        addDockWidget(Qt::RightDockWidgetArea, dock);
        ui->action_Debug->setChecked(true);
    }
}


/*
 * Callback for loading a data file which is dropped into the window
 */
void MainWindow::loadDroppedFile(QString filename)
{
    qDebug() << "MainWindow::loadDroppedFile" << filename;
    loadDataFromFile(filename);
}


/*
 * Construct a list of available data file importer classes
 */
QList<QSharedPointer<FileDataSource>> MainWindow::getFileImporters()
{
    // TODO: In the future this should hook into the plugin architecture

    QList<QSharedPointer<FileDataSource>> sources;

    sources.append(QSharedPointer<FileDataSource>(new CSVImporter()));
    sources.append(QSharedPointer<FileDataSource>(new CEDATImporter()));
    sources.append(QSharedPointer<FileDataSource>(new MavlinkImporter()));

    return sources;
}


void MainWindow::loadDataFromFile(QString filename)
{
    qDebug() << "loadDataFromFile:" << filename;

    auto *settings = LumberjackSettings::getInstance();

    // Record the directory this file was loaded from
    QFileInfo fi(filename);
    settings->saveSetting("import", "lastDirectory", fi.absoluteDir().absolutePath());

    auto sources = getFileImporters();

    QStringList errors;
    bool matched = false;
    bool loaded = false;

    for (auto source : sources)
    {
        if (source.isNull()) continue;

        if (source->supportsFileType(fi.suffix()))
        {
            matched = true;

            loaded = source->loadData(filename, errors);

            if (loaded)
            {
                DataSourceManager::getInstance()->addSource(source);
                break;
            }
        }
    }

    if (!matched)
    {
        qWarning() << "No importer class found for" << filename;
    }
    else if (!loaded)
    {
        qCritical() << "File could not be loaded" << filename;
    }

    if (errors.length() > 0)
    {
        for (auto error : errors)
        {
            qWarning() << error;
        }
    }
}


/*
 * Callback when the "import data" menu action is fired
 */
void MainWindow::importData()
{
    auto *settings = LumberjackSettings::getInstance();

    qDebug() << "MainWindow::importData";

    auto sources = getFileImporters();

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

    QString lastDir = settings->loadSetting("import", "lastDirectory", QString()).toString();

    if (!lastDir.isEmpty())
    {
        dialog.setDirectory(lastDir);
    }

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

    loadDataFromFile(filename);
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


/*
 * Callback when a DataSeries is removed from the available graphs
 */
void MainWindow::seriesRemoved(QSharedPointer<DataSeries> series)
{
    if (series.isNull()) return;

    for (auto plot : plots)
    {
        if (plot.isNull()) continue;

        plot->removeSeries(series);
    }
}


/*
 * Add a new plot widget to the display
 */
void MainWindow::addPlot()
{
    qDebug() << "MainWindow::addPlot()";

    PlotWidget *plot = new PlotWidget();

    plot->setParent(this);

    plotLayout.addWidget(plot);

    // Connect signals/slots for the new plot
    connect(plot, &PlotWidget::cursorPositionChanged, this, &MainWindow::updateCursorPos);
    connect(plot, &PlotWidget::viewChanged, this, &MainWindow::onTimescaleChanged);
    connect(plot, &PlotWidget::viewChanged, &timelineView, &TimelineWidget::updateViewLimits);
    connect(plot, &PlotWidget::timestampLimitsChanged, &timelineView, &TimelineWidget::updateTimeLimits);
    connect(plot, &PlotWidget::fileDropped, this, &MainWindow::loadDroppedFile);

    plots.append(QSharedPointer<PlotWidget>(plot));
}


/*
 * Remove the specified plot from the display
 */
void MainWindow::removePlot(QSharedPointer<PlotWidget> plot)
{
    Q_UNUSED(plot)
    // TODO
}


void MainWindow::toggleFftView(void)
{
    ui->action_FFT->setCheckable(true);

    if (fftView.isVisible())
    {
        hideDockedWidget(&fftView);
        ui->action_FFT->setChecked(false);
    }
    else
    {
        QDockWidget* dock = new QDockWidget(tr("FFT View"), this);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(&fftView);

        addDockWidget(Qt::LeftDockWidgetArea, dock);

        ui->action_Data_View->setChecked(true);
    }
}


/**
 * @brief MainWindow::toggleDataView toggles visibility of the "data view" dock
 */
void MainWindow::toggleDataView(void)
{
    ui->action_Data_View->setCheckable(true);

    if (dataView.isVisible())
    {
        hideDockedWidget(&dataView);

        ui->action_Data_View->setChecked(false);
    }
    else
    {
        QDockWidget* dock = new QDockWidget(tr("Data View"), this);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(&dataView);

        addDockWidget(Qt::LeftDockWidgetArea, dock);

        ui->action_Data_View->setChecked(true);
    }
}


/**
 * @brief MainWindow::toggleTimelineView toggles visibility of the "timeline" dock
 */
void MainWindow::toggleTimelineView(void)
{
    ui->action_Timeline->setCheckable(true);

    if (timelineView.isVisible())
    {
        hideDockedWidget(&timelineView);

        ui->action_Timeline->setChecked(false);
    }
    else
    {
        QDockWidget* dock = new QDockWidget(tr("Timeline"), this);
        dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        dock->setWidget(&timelineView);

        addDockWidget(Qt::BottomDockWidgetArea, dock);

        ui->action_Timeline->setChecked(true);
    }
}


/**
 * @brief MainWindow::toggleStatisticsView toggles visibility of the "statistics" dock
 */
void MainWindow::toggleStatisticsView(void)
{
    ui->action_Statistics->setCheckable(true);

    if (statsView.isVisible())
    {
        hideDockedWidget(&statsView);

        ui->action_Statistics->setChecked(false);
    }
    else
    {
        QDockWidget *dock = new QDockWidget(tr("Stats View"), this);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setWidget(&statsView);

        addDockWidget(Qt::LeftDockWidgetArea, dock);

        ui->action_Statistics->setChecked(true);
    }
}
