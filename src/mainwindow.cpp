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

#include "plugins_dialog.hpp"
#include "about_dialog.hpp"

#include "plugin_registry.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>


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

    // Set a null central widget - required to allow fullscreen docks
    setCentralWidget(NULL);

    // Add an initial plot
    addPlot();

    setAcceptDrops(true);

    loadWorkspaceSettings();

    pluginRegistry.loadPlugins();
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
    /*
     * TODO: Reimplement this as a plugin
     */

    /*

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
    auto series_5 = QSharedPointer<DataSeries>(new DataSeries("Series 5"));
    auto series_6 = QSharedPointer<DataSeries>(new DataSeries("Series 6"));
    auto series_7 = QSharedPointer<DataSeries>(new DataSeries("Series 7"));

    for (double t = 0; t < 100; t += 0.0001)
    {        
        // 5Hz, 20Hz, 50Hz
        series_2->addData(t, 10 * cos(5 * 2 * M_PI * t) + 5 * sin(20 * 2 * M_PI * t) + 15 * sin(50 * 2 * M_PI * t));
        series_3->addData(t, 10 * sin(15 * t));

        // Random time data wobble
        double t_offset = (double) (rand() % 1000 - 500) / 1000;
        t_offset *= 0.0001;

        series_4->addData(t + t_offset, 15 * cos(5 * t));

        // Series 5, 6, 7 have the same frequency data, but at different sampling period
        // This ensures that the FFT calculations are correct

        // FFT should have peak at 123.45Hz
        const double f_base = 123.45 * 2 * M_PI;

        series_5->addData(0.1 * t, 10 * cos(0.1 * t * f_base));
        series_6->addData(1.0 * t, 10 * cos(1.0 * t * f_base));
        series_7->addData(10  * t, 10 * cos(10  * t * f_base));
    }

    src->addSeries(series_2);
    src->addSeries(series_3);
    src->addSeries(series_4);
    src->addSeries(series_5);
    src->addSeries(series_6);
    src->addSeries(series_7);
    */
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
    connect(ui->action_Plugins, &QAction::triggered, this, &MainWindow::showPluginsInfo);
    connect(ui->action_Debug, &QAction::triggered, this, &MainWindow::toggleDebugView);
}


/**
 * @brief MainWindow::initDocks initializes the various dockable widgets
 */
void MainWindow::initDocks()
{
    this->setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks);

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

    // Update the "fft" view
    fftView.updateInterval(viewInterval);
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
 * Display the "Plugins" dialog
 */
void MainWindow::showPluginsInfo(void)
{
    PluginsDialog dlg(pluginRegistry, this);
    dlg.exec();
}


/*
 * Display the "About" dialog
 */
void MainWindow::showAboutInfo()
{
    AboutDialog dlg(this);

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



/**
 * @brief MainWindow::loadDataFromFile - Load data from the provided file
 * @param filename
 */
void MainWindow::loadDataFromFile(QString filename)
{
    qDebug() << "loadDataFromFile:" << filename;

    auto *settings = LumberjackSettings::getInstance();

    // Record the directory this file was loaded from
    QFileInfo fi(filename);

    if (!fi.exists())
    {
        // TODO - show error message
        return;
    }

    settings->saveSetting("import", "lastDirectory", fi.absoluteDir().absolutePath());

    // Find a matching plugin
    ImportPluginList importers;

    QSharedPointer<ImportPlugin> importer;

    for (auto plugin : pluginRegistry.ImportPlugins())
    {
        if (plugin.isNull()) continue;

        if (plugin->supportsFileType(fi.suffix()))
        {
            importers.append(plugin);
        }
    }

    if (importers.length() == 1)
    {
        importer = importers.first();
    }
    else if (importers.length() == 0)
    {
        // TODO: Select an importer
        // TODO: For now, just take the first one...
        importer = importers.first();
    }
    else
    {
        // TODO: Select an importer
        // TODO: For now, just take the first one...
        importer = importers.first();
    }

    // Create a new instance of the provided importer
    DataSource *source = new DataSource(importer->pluginName(), importer->pluginDescription());

    QStringList errors;


    if (!importer->validateFile(filename, errors))
    {
        // TODO: error message?
        return;
    }

    importer->setFilename(filename);

    if (!importer->beforeLoadData())
    {
        // TODO: error message?
        return;
    }

    errors.clear();

    bool result = importer->loadDataFile(errors);

    if (!result)
    {
        // TODO: Display errors

        delete source;
        return;
    }

    auto seriesList = importer->getDataSeries();

    if (seriesList.count() == 0)
    {
        // TODO: Error msg - no data imported
        delete source;
        return;
    }

    for (auto series : importer->getDataSeries())
    {
        source->addSeries(series);
    }

    DataSourceManager::getInstance()->addSource(source);

    // TODO: Success message?
}


/*
 * Callback when the "import data" menu action is fired
 */
void MainWindow::importData()
{
    auto *settings = LumberjackSettings::getInstance();

    qDebug() << "MainWindow::importData";

    // Assemble set of supported file types
    QStringList supportedFileTypes;

    QStringList filePatterns;

    for (QSharedPointer<ImportPlugin> plugin : pluginRegistry.ImportPlugins())
    {
        if (plugin.isNull()) continue;

        filePatterns.append(plugin->fileFilter());
    }

    filePatterns.append("Any files (*)");

    // Load a file
    QFileDialog dialog(this);

    dialog.setWindowTitle(tr("Import Data from File"));

    QString lastDir = settings->loadSetting("import", "lastDirectory", QString()).toString();

    if (!lastDir.isEmpty())
    {
        dialog.setDirectory(lastDir);
    }

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters(filePatterns);
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
    static int plotIndex = 1;

    qDebug() << "MainWindow::addPlot()";

    PlotWidget *plot = new PlotWidget();

    QDockWidget *dock = new QDockWidget(tr("Plot") + QString(" ") + QString::number(plotIndex), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    plot->setParent(dock);
    dock->setWidget(plot);

    // Ensure a sensible minimum size
    plot->setMinimumSize(200, 100);

    addDockWidget(Qt::TopDockWidgetArea, dock, Qt::Vertical);

    // Connect signals/slots for the new plot
    connect(plot, &PlotWidget::cursorPositionChanged, this, &MainWindow::updateCursorPos);
    connect(plot, &PlotWidget::viewChanged, this, &MainWindow::onTimescaleChanged);
    connect(plot, &PlotWidget::viewChanged, &timelineView, &TimelineWidget::updateViewLimits);
    connect(plot, &PlotWidget::timestampLimitsChanged, &timelineView, &TimelineWidget::updateTimeLimits);
    connect(plot, &PlotWidget::fileDropped, this, &MainWindow::loadDroppedFile);

    plots.append(QSharedPointer<PlotWidget>(plot));

    plotIndex++;
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
