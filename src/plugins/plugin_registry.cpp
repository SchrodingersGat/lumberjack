#include <QCoreApplication>
#include <QApplication>
#include <QFileDialog>
#include <QDialog>
#include <QPluginLoader>
#include <QDir>

#include "plugin_registry.hpp"
#include "lumberjack_settings.hpp"

PluginRegistry* PluginRegistry::instance = 0;


PluginRegistry::PluginRegistry() : QObject()
{
}


PluginRegistry::~PluginRegistry()
{
    clearRegistry();
}


/*
 * Load all dynamic plugins
 * - Iterates through specified plugin dirs
 */
void PluginRegistry::loadPlugins()
{
    QList<PluginBase*> pluginInstances;

    QList<QString> checkedPaths;

    for (QString libPath : QApplication::libraryPaths())
    {
        if (checkedPaths.contains(libPath)) continue;

        checkedPaths.append(libPath);

        QDir libDir(libPath);

        if (!libDir.exists()) continue;

        for (QString libFile : libDir.entryList(QDir::Files))
        {
            QString libPath = libDir.absoluteFilePath(libFile);

            QPluginLoader loader(libPath);

            QObject *instance = loader.instance();

            // Try to load against each type of plugin interface
            if      (loadImportPlugin(instance)) {}
            else if (loadExportPlugin(instance)) {}
            else if (loadFilterPlugin(instance)) {}
            else
            {
                // No match for the plugin
                delete instance;
            }
        }
    }
}


void PluginRegistry::clearRegistry()
{
    m_ImportPlugins.clear();
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
