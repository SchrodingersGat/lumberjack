#ifndef PLUGIN_REGISTRY_HPP
#define PLUGIN_REGISTRY_HPP

#include <QObject>

#include "plugin_importer.hpp"
#include "plugin_exporter.hpp"

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

    const ImporterPluginList& importerPlugins(void) { return m_importerPlugins; }
    const ExporterPluginList& exporterPlugins(void) { return m_exporterPlugins; }

protected:

    bool loadImporterPlugin(QObject *instance);
    bool loadExporterPlugin(QObject *instance);

    // Registry of each plugin "type"
    ImporterPluginList m_importerPlugins;
    ExporterPluginList m_exporterPlugins;

};

};

#endif // PLUGIN_REGISTRY_HPP
