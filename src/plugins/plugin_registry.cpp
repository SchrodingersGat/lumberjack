#include <QCoreApplication>
#include <QApplication>
#include <QPluginLoader>
#include <QDir>

#include "plugin_registry.hpp"

PluginRegistry::PluginRegistry(QObject *parent) : QObject(parent)
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

    for (QString libPath : QApplication::libraryPaths())
    {
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
            else
            {
                // No match for the plugin
                delete instance;
            }
        }
    }

    qDebug() << "Loading plugins:";
    qDebug() << "Importer:" << m_ImportPlugins.count();
    qDebug() << "Exporter:" << m_ExportPlugins.count();
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
