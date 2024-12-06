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
    QList<QObject*> pluginInstances;

    for (QString libPath : QApplication::libraryPaths())
    {
        QDir libDir(libPath);

        if (!libDir.exists()) continue;

        qDebug() << "Checking plugin dir:" << libPath;


        for (QString libFile : libDir.entryList(QDir::Files))
        {
            QString libPath = libDir.absoluteFilePath(libFile);

            QPluginLoader loader(libPath);

            QObject *instance = loader.instance();

            if (instance)
            {
                qDebug() << "- Found plugin:" << libFile;

                pluginInstances.append(instance);
            }
        }
    }

    // TODO: Actually.. "do" something with these
    qDeleteAll(pluginInstances);
}


void PluginRegistry::clearRegistry()
{
    importerPlugins.clear();
}


};
