#include <QCoreApplication>
#include <QApplication>
#include <QFileDialog>
#include <QDialog>
#include <QPluginLoader>
#include <QLibrary>
#include <QDir>
#include <QDebug>
#include <QJsonObject>

#include "plugin_registry.hpp"
#include "lumberjack_settings.hpp"

// Imports for built-in plugin classes
#include "plugins/csv_importer/lumberjack_csv_importer.hpp"
#include "plugins/csv_exporter/lumberjack_csv_exporter.hpp"
#include "plugins/offset_filter/offset_filter.hpp"
#include "plugins/scaler_filter/scaler_filter.hpp"

/**
 * @brief Check whether a plugin was built against the same Qt version and
 * build type (debug/release) as this application.
 *
 * QPluginLoader/QLibrary's own compatibility check is coarser than this: it can
 * accept a plugin built against a different (but "close enough") Qt version, and
 * does not reliably reject a debug/release mismatch on every platform. Loading an
 * ABI-incompatible plugin doesn't fail cleanly in that case - it corrupts memory
 * the moment the plugin's code actually runs. Require an exact match instead.
 */
static bool isPluginCompatible(const QString &pluginPath)
{
    QPluginLoader loader(pluginPath);

    QJsonObject metaData = loader.metaData();

    if (metaData.isEmpty())
    {
        // Not a Qt plugin at all
        return false;
    }

#if defined(QT_NO_DEBUG)
    const bool hostIsDebugBuild = false;
#else
    const bool hostIsDebugBuild = true;
#endif

    const int pluginVersion = metaData.value("version").toInt();
    const bool pluginIsDebugBuild = metaData.value("debug").toBool();

    if (pluginVersion != QT_VERSION || pluginIsDebugBuild != hostIsDebugBuild)
    {
        qWarning().noquote() << QString(
            "Skipping incompatible plugin '%1': built against Qt %2.%3.%4 (%5), "
            "but this application is Qt %6 (%7)")
            .arg(pluginPath)
            .arg((pluginVersion >> 16) & 0xFF)
            .arg((pluginVersion >> 8) & 0xFF)
            .arg(pluginVersion & 0xFF)
            .arg(pluginIsDebugBuild ? "debug" : "release")
            .arg(QT_VERSION_STR)
            .arg(hostIsDebugBuild ? "debug" : "release");

        return false;
    }

    return true;
}

PluginRegistry* PluginRegistry::instance = 0;


PluginRegistry::PluginRegistry() : QObject()
{
}


PluginRegistry::~PluginRegistry()
{
    clearRegistry();
}


/**
 * Load a set of "default" or "builtin" plugins
 * These plugins are distributed with the source code
 */
void PluginRegistry::loadBuiltinPlugins()
{
    // Builtin importer plugins
    m_ImportPlugins.append(QSharedPointer<ImportPlugin>(new LumberjackCSVImporter()));

    // Builtin exporter plugins
    m_ExportPlugins.append(QSharedPointer<ExportPlugin>(new LumberjackCSVExporter()));

    // Builtin filter plugins
    m_FilterPlugins.append(QSharedPointer<FilterPlugin>(new OffsetFilter()));
    m_FilterPlugins.append(QSharedPointer<FilterPlugin>(new ScalerFilter()));
}


/*
 * Load all dynamic plugins
 * - Iterates through specified plugin dirs
 */
void PluginRegistry::loadPlugins()
{

    loadBuiltinPlugins();

    QList<QString> checkedPaths;

    for (QString libPath : QApplication::libraryPaths())
    {
        if (checkedPaths.contains(libPath)) continue;

        checkedPaths.append(libPath);

        QDir libDir(libPath);

        if (!libDir.exists()) continue;

        for (QString libFile : libDir.entryList(QDir::Files))
        {
            QString pluginPath = libDir.absoluteFilePath(libFile);

            // Skip anything that isn't a shared library (e.g. our own .exe,
            // Qt/runtime DLLs sitting alongside it, stray non-binary files)
            if (!QLibrary::isLibrary(pluginPath)) continue;

            if (!isPluginCompatible(pluginPath)) continue;

            QPluginLoader loader(pluginPath);

            QObject *instance = loader.instance();

            if (!instance)
            {
                qWarning() << "Failed to load plugin" << pluginPath << ":" << loader.errorString();
                continue;
            }

            // Try to load against each type of plugin interface
            if      (loadImportPlugin(instance)) {}
            else if (loadExportPlugin(instance)) {}
            else if (loadFilterPlugin(instance)) {}
            else
            {
                // Loaded successfully, but doesn't implement a known interface.
                // The instance is owned by QPluginLoader - don't delete it ourselves,
                // just unload the library.
                qWarning() << "Plugin" << pluginPath << "does not implement a known plugin interface";
                loader.unload();
            }
        }
    }
}


void PluginRegistry::clearRegistry()
{
    m_ImportPlugins.clear();
    m_ExportPlugins.clear();
    m_FilterPlugins.clear();
}


/**
 * @brief PluginRegistry::loadImportPlugin - Attempt to load an importer plugin
 * @param instance
 * @return
 */
bool PluginRegistry::loadImportPlugin(QObject *instance)
{
    ImportPlugin *plugin = qobject_cast<ImportPlugin*>(instance);

    if (plugin)
    {
        m_ImportPlugins.append(QSharedPointer<ImportPlugin>(plugin));
        return true;
    }

    return false;
}


bool PluginRegistry::loadExportPlugin(QObject *instance)
{
    ExportPlugin *plugin = qobject_cast<ExportPlugin*>(instance);

    if (plugin)
    {
        m_ExportPlugins.append(QSharedPointer<ExportPlugin>(plugin));
        return true;
    }

    return false;
}


bool PluginRegistry::loadFilterPlugin(QObject *instance)
{
    FilterPlugin *plugin = qobject_cast<FilterPlugin*>(instance);

    if (plugin)
    {
        m_FilterPlugins.append(QSharedPointer<FilterPlugin>(plugin));
        return true;
    }

    return false;
}


/**
 * @brief PluginRegistry::getFilenameForImport - Select a file for importing
 * @return
 */
QString PluginRegistry::getFilenameForImport(void) const
{
    auto settings = LumberjackSettings::getInstance();

    QStringList filePatterns;

    for (QSharedPointer<ImportPlugin> plugin : m_ImportPlugins)
    {
        if (plugin.isNull()) continue;

        filePatterns.append(plugin->fileFilter());
    }

    filePatterns.append("Any files (*)");

    QFileDialog dialog;

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
        return QString();
    }

    // Determine which plugin loaded the data
    QString filter = dialog.selectedNameFilter();
    QStringList files = dialog.selectedFiles();

    QString filename;

    if (!filter.isEmpty() && files.length() >= 1)
    {
        filename = files.first();
    }

    return filename;
}


QString PluginRegistry::getFilenameForExport(void) const
{
    auto settings = LumberjackSettings::getInstance();

    QStringList filePatterns;

    for (QSharedPointer<ExportPlugin> plugin : m_ExportPlugins)
    {
        if (plugin.isNull()) continue;

        filePatterns.append(plugin->fileFilter());
    }

    filePatterns.append("Any Files (*)");

    QFileDialog dialog;

    dialog.setWindowTitle(tr("Export Data to File"));

    QString lastDir = settings->loadSetting("export", "lastDirectory", QString()).toString();

    if (!lastDir.isEmpty())
    {
        dialog.setDirectory(lastDir);
    }

    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilters(filePatterns);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setLabelText(QFileDialog::DialogLabel::Accept, tr("Export"));

    int result = dialog.exec();

    if (result != QDialog::Accepted)
    {
        return QString();
    }

    QString filter = dialog.selectedNameFilter();
    QStringList files = dialog.selectedFiles();

    QString filename;

    if (!filter.isEmpty() && files.length() >= 1)
    {
        filename = files.first();
    }

    return filename;
}
