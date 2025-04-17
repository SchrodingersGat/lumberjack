#ifndef PLUGIN_REGISTRY_HPP
#define PLUGIN_REGISTRY_HPP

#include "plugin_filter.hpp"
#include "plugin_importer.hpp"
#include "plugin_exporter.hpp"

/**
 * @brief The PluginRegistry class manages loading of custom plugins
 */
class PluginRegistry : public QObject
{
    Q_OBJECT
    static PluginRegistry* instance;

public:
    PluginRegistry();
    virtual ~PluginRegistry();

    static PluginRegistry* getInstance()
    {
        if (!instance)
        {
            instance = new PluginRegistry();
        }

        return instance;
    }

    static void cleanup()
    {
        if (instance)
        {
            instance->clearRegistry();
            delete instance;
            instance = nullptr;
        }
    }

    void loadPlugins(void);
    void loadBuiltinPlugins(void);
    void clearRegistry(void);

    const ImportPluginList& ImportPlugins(void) { return m_ImportPlugins; }
    const ExportPluginList& ExportPlugins(void) { return m_ExportPlugins; }
    const FilterPluginList& FilterPlugins(void) { return m_FilterPlugins; }

    QString getFilenameForImport(void) const;
    QString getFilenameForExport(void) const;

protected:

    bool loadImportPlugin(QObject *instance);
    bool loadExportPlugin(QObject *instance);
    bool loadFilterPlugin(QObject *instance);

    // Registry of each plugin "type"
    ImportPluginList m_ImportPlugins;
    ExportPluginList m_ExportPlugins;
    FilterPluginList m_FilterPlugins;

};

#endif // PLUGIN_REGISTRY_HPP
