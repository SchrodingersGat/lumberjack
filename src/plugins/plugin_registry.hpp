#ifndef PLUGIN_REGISTRY_HPP
#define PLUGIN_REGISTRY_HPP

#include <QObject>

#include "plugin_importer.hpp"

namespace Lumberjack {

/**
 * @brief The PluginRegistry class manages loading of custom plugins
 */
class PluginRegistry : public QObject
{
    Q_OBJECT
public:
    PluginRegistry(QObject *parent = nullptr);
    virtual ~PluginRegistry();

    void loadPlugins(void);
    void clearRegistry(void);

protected:

    bool loadImporterPlugin(QObject *instance);

    // Registry of each plugin "type"
    QList<QSharedPointer<ImporterPlugin>> importerPlugins;

};

};

#endif // PLUGIN_REGISTRY_HPP
