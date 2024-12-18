#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>
#include <QApplication>
#include <QThread>

#include "data_source_manager.hpp"

#include "plugin_registry.hpp"
#include "lumberjack_settings.hpp"



DataImportWorker::DataImportWorker(QSharedPointer<ImportPlugin> plugin) : m_plugin(plugin)
{
}


void DataImportWorker::runImport()
{
    m_errors.clear();

    if (m_plugin)
    {
        m_result = m_plugin->importData(m_errors);
    }
    else
    {
        m_result = false;
    }

    m_complete = true;

    emit importCompleted();
}


void DataImportWorker::cancelImport()
{
    if (m_plugin)
    {
        m_plugin->cancelProcessing();
        m_result = false;
    }

    m_errors.append(tr("Import process cancelled"));

    m_complete = true;
}


DataExportWorker::DataExportWorker(QSharedPointer<ExportPlugin> plugin, QList<DataSeriesPointer> &series)
    : m_plugin(plugin), m_series(series)
{
}


void DataExportWorker::runExport()
{
    m_errors.clear();

    if (m_plugin)
    {
        m_result = m_plugin->exportData(m_series, m_errors);
    }
    else
    {
        m_result = false;
    }

    m_complete = true;
    emit exportCompleted();
}


void DataExportWorker::cancelExport()
{
    if (m_plugin)
    {
        m_plugin->cancelProcessing();
        m_result = false;
    }

    m_errors.append(tr("Export process cancelled"));

    m_complete = true;
}


DataSourceManager *DataSourceManager::instance = 0;


DataSourceManager::DataSourceManager()
{
}


DataSourceManager::~DataSourceManager()
{
    removeAllSources(false);
}


DataSeriesPointer DataSourceManager::findSeries(QString source_label, QString series_label)
{
    auto source = getSourceByLabel(source_label);

    if (source.isNull()) return DataSeriesPointer(nullptr);

    return source->getSeriesByLabel(series_label);
}


QStringList DataSourceManager::getSourceLabels() const
{
    QStringList labels;

    for (auto src : sources)
    {
        if (src.isNull()) continue;

        labels.push_back(src->getLabel());
    }

    return labels;
}


DataSourcePointer DataSourceManager::getSourceByIndex(unsigned int idx)
{
    if (idx < sources.size()) return sources.at(idx);

    return DataSourcePointer(nullptr);
}


DataSourcePointer DataSourceManager::getSourceByLabel(QString label)
{
    for (auto src : sources)
    {
        if (src.isNull()) continue;

        if (src->getLabel() == label)
        {
            return src;
        }
    }

    return DataSourcePointer(nullptr);
}


bool DataSourceManager::addSource(DataSourcePointer source)
{
    if (source.isNull()) return false;

    QString identifier = source->getIdentifier();

    for (auto src : sources)
    {
        if (src.isNull()) continue;

        if (src == source || src->getIdentifier() == identifier)
        {
            qInfo() << "Ignoring duplicate source:" << source->getLabel();

            return false;
        }
    }

    sources.push_back(source);

    connect(source.data(), &DataSource::dataChanged, this, &DataSourceManager::onDataChanged);

    emit sourcesChanged();

    return true;
}


/**
 * @brief DataSourceManager::addSource adds a DataSource with the given title
 * @param label is the label of the DataSource to add
 * @return true if the source was added, false if a DataSource with the given title already existed
 */
bool DataSourceManager::addSource(QString source, QString label, QString description)
{
    if (getSourceByLabel(label).isNull())
    {
        return addSource(new DataSource(source, label, description));
    }

    // Source with provided label already exists!
    return false;
}


/**
 * @brief DataSourceManager::removeSource removes the DataSource from this DataSourceManager
 * @param source is a shared pointer to the DataSource
 * @return true if the source was removed, else false
 */
bool DataSourceManager::removeSource(DataSourcePointer source)
{
    for (auto idx = 0; idx < sources.size(); idx++)
    {
        auto src = sources.at(idx);

        if (src == source)
        {
            sources.removeAt(idx);
            emit sourcesChanged();
            return true;
        }
    }

    return false;
}


/**
 * @brief DataSourceManager::removeSourceByIndex removes the DataSource at the specified index
 * @param idx - Index of the DataSource to remove
 * @param update - If true, emit a "sourcesChanged" event
 * @return true if the source was removed, else false (in the case of an invalid index)
 */
bool DataSourceManager::removeSourceByIndex(unsigned int idx, bool update)
{
    if (idx < sources.size())
    {
        sources.removeAt(idx);

        if (update)
        {
            emit sourcesChanged();
        }

        return true;
    }

    return false;
}


/**
 * @brief DataSourceManager::removeSourceByLabel - Remove the first DataSource which matches the specified label
 * @param label - QString label
 * @param update - If true, emit a "sourcesChanged" event
 * @return true if a source matching the provided label was found (and subsequentyly deleted)
 */
bool DataSourceManager::removeSourceByLabel(QString label, bool update)
{
    for (int idx = 0; idx < sources.size(); idx++)
    {
        auto src = sources.at(idx);

        if (!src.isNull() && src->getLabel() == label)
        {
            return removeSourceByIndex(idx, update);
        }
    }

    return false;
}


/**
 * @brief DataSourceManager::removeAllSources removes all DataSource objects from this DataSourceManager
 * @param update - if true, emit a "sourcesChanged" event
 */
void DataSourceManager::removeAllSources(bool update)
{
    while (sources.count() > 0)
    {
        removeSourceByIndex(0, update);
    }

    if (update)
    {
        emit sourcesChanged();
    }
}


bool DataSourceManager::importData(QString filename)
{
    auto registry = PluginRegistry::getInstance();
    auto settings = LumberjackSettings::getInstance();

    if (filename.isEmpty())
    {
        filename = registry->getFilenameForImport();
    }

    // Still empty? No further actions
    if (filename.isEmpty())
    {
        return false;
    }

    QFileInfo fi(filename);

    if (!fi.exists())
    {
        qCritical() << "File does not exist:" << filename;
        return false;
    }

    // Save the last directory information
    settings->saveSetting("import", "lastDirectory", fi.absoluteDir().absolutePath());

    ImportPluginList importers;

    for (auto plugin : registry->ImportPlugins())
    {
        if (plugin.isNull()) continue;

        if (plugin->supportsFileType(fi.suffix()))
        {
            importers.append(plugin);
        }
    }

    QSharedPointer<ImportPlugin> importer;

    if (importers.length() == 0)
    {
        // TODO: Error message
        return false;
    }
    else if (importers.length() == 1)
    {
        importer = importers.first();
    }
    else
    {
        // TODO: Select an importer
        // TODO: For now, just take the first one...
        importer = importers.first();
    }

    QStringList errors;

    if (!importer->validateFile(filename, errors))
    {
        // TODO: Display errors

        qWarning() << "File is not valid:" << filename;
        return false;
    }

    importer->setFilename(filename);

    if (!importer->beforeProcessStep())
    {
        // TODO: error message?
        return false;
    }

    QProgressDialog progress;

    progress.setWindowTitle(tr("Importing Data"));
    progress.setMinimum(0);
    progress.setMaximum(100);
    progress.setValue(0);
    progress.setLabelText(tr("Importing data from file"));

    progress.show();

    QApplication::processEvents();

    // Spawn a new thread for importing
    auto worker = DataImportWorker(importer);
    auto *thread = new QThread;

    worker.moveToThread(thread);

    connect(&worker, &DataImportWorker::importCompleted, thread, &QThread::quit);
    connect(thread, &QThread::started, &worker, &DataImportWorker::runImport);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    qDebug() << "Importing data from" << filename;

    while (!thread->isFinished() && !worker.isComplete())
    {
        // Check for manual cancel of import process
        if (progress.wasCanceled())
        {
            worker.cancelImport();
            thread->wait();
        }

        progress.setValue(importer->getProgress());

        QApplication::processEvents();
        QThread::msleep(100);
    }

    progress.cancel();
    progress.close();

    for (QString err : worker.getErrors())
    {
        // TODO: Display these better?
        qWarning() << "Import err:" << err;
    }

    if (worker.getResult())
    {
        // Create a new instance of the provided importer
        DataSource *source = new DataSource(
            importer->pluginName(),
            fi.fileName(),
            fi.absoluteFilePath()
        );


        auto seriesList = importer->getDataSeries();

        if (seriesList.count() == 0)
        {
            delete source;
            return false;
        }

        for (auto series : importer->getDataSeries())
        {
            source->addSeries(series);
        }

        addSource(source);
    }

    return true;
}


/**
 * @brief DataSourceManager::exportData - Export a set of data series to a file
 * @param series
 * @param filename
 * @return
 */
bool DataSourceManager::exportData(QList<DataSeriesPointer> &series, QString filename)
{
    auto registry = PluginRegistry::getInstance();
    auto settings = LumberjackSettings::getInstance();

    if (filename.isEmpty())
    {
        filename = registry->getFilenameForExport();
    }

    // Still empty? No further progress
    if (filename.isEmpty())
    {
        return false;
    }

    QFileInfo fi(filename);

    // Save the last directory information
    settings->saveSetting("export", "lastDirectory", fi.absoluteDir().absolutePath());

    ExportPluginList exporters;

    QSharedPointer<ExportPlugin> exporter;

    for (auto plugin : registry->ExportPlugins())
    {
        if (plugin.isNull()) continue;

        if (plugin->supportsFileType(fi.suffix()))
        {
            exporters.append(plugin);
        }
    }

    if (exporters.length() == 0)
    {
        // TODO: Error message
        return false;
    }
    else if (exporters.length() == 1)
    {
        exporter = exporters.first();
    }
    else
    {
        // TODO: select an exporter
        // TODO: For now, just take the first one
        exporter = exporters.first();
    }

    exporter->setFilename(filename);

    if (!exporter->beforeProcessStep())
    {
        // TODO: error mesage?
        return false;
    }

    QProgressDialog progress;

    progress.setWindowTitle(tr("Importing Data"));
    progress.setMinimum(0);
    progress.setMaximum(100);
    progress.setValue(0);
    progress.setLabelText(tr("Importing data from file"));

    progress.show();

    QApplication::processEvents();

    // Spawn a new thread for data export
    auto worker = DataExportWorker(exporter, series);
    auto *thread = new QThread();

    worker.moveToThread(thread);

    connect(&worker, &DataExportWorker::exportCompleted, thread, &QThread::quit);
    connect(thread, &QThread::started, &worker, &DataExportWorker::runExport);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    qDebug() << "Exporting data to" << filename;

    while (!thread->isFinished() && !worker.isComplete())
    {
        if (progress.wasCanceled())
        {
            worker.cancelExport();
            thread->wait();
        }

        progress.setValue(exporter->getProgress());

        QApplication::processEvents();
        QThread::msleep(100);
    }

    progress.cancel();
    progress.close();

    for (QString err : worker.getErrors())
    {
        // TODO: Display these better?
        qWarning() << "Export err:" << err;
    }

    bool result = worker.getResult();

    return result;
}
