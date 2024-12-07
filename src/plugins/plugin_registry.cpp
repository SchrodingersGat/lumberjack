#include <QCoreApplication>
#include <QApplication>
#include <QPluginLoader>
#include <QDir>

#include "plugin_registry.hpp"

namespace Lumberjack {

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
            if (loadImporterPlugin(instance)) {}
            else
            {
                // No match for the plugin
                delete instance;
            }
        }
    }

    qDebug() << "Loading plugins:";
    qDebug() << "Importer:" << importerPlugins.count();
}


void PluginRegistry::clearRegistry()
{
    importerPlugins.clear();
}


/**
 * @brief PluginRegistry::loadImporterPlugin - Attempt to load an importer plugin
 * @param instance
 * @return
 */
bool PluginRegistry::loadImporterPlugin(QObject *instance)
{
    ImporterInterface *plugin = qobject_cast<ImporterInterface*>(instance);

    if (plugin)
    {
        importerPlugins.append(QSharedPointer<ImporterInterface>(plugin));
        return true;
    }

    return false;
}


};
