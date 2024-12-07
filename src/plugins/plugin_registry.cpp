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
    qDebug() << "Importer:" << m_importerPlugins.count();
}


void PluginRegistry::clearRegistry()
{
    m_importerPlugins.clear();
}


/**
 * @brief PluginRegistry::loadImporterPlugin - Attempt to load an importer plugin
 * @param instance
 * @return
 */
bool PluginRegistry::loadImporterPlugin(QObject *instance)
{
    ImporterPlugin *plugin = qobject_cast<ImporterPlugin*>(instance);

    if (plugin)
    {
        m_importerPlugins.append(QSharedPointer<ImporterPlugin>(plugin));
        return true;
    }

    return false;
}

};
