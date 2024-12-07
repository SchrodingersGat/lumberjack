#ifndef PLUGIN_REGISTRY_HPP
#define PLUGIN_REGISTRY_HPP

#include <QObject>

#include "plugin_importer.hpp"
#include "plugin_exporter.hpp"

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

    const ImportPluginList& ImportPlugins(void) { return m_ImportPlugins; }
    const ExportPluginList& ExportPlugins(void) { return m_ExportPlugins; }

protected:

    bool loadImportPlugin(QObject *instance);
    bool loadExportPlugin(QObject *instance);

    // Registry of each plugin "type"
    ImportPluginList m_ImportPlugins;
    ExportPluginList m_ExportPlugins;

};

#endif // PLUGIN_REGISTRY_HPP
