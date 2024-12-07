#ifndef PLUGIN_EXPORTER_HPP
#define PLUGIN_EXPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

#define ExporterInterface_iid "org.lumberjack.plugins.ExportPlugin/1.0"

/**
 * @brief The ExportPlugin class defines an interface for exporting data
 */
class ExportPlugin : public PluginBase
{
    Q_OBJECT
public:
    virtual ~ExportPlugin() = default;

    // Generate a new ExportPlugin interface
    virtual ExportPlugin* newInstance(void) const = 0;

    // Return a list of the support file types e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    // TODO: Export data function

    virtual QString pluginIID(void) const override
    {
        return QString(ExporterInterface_iid);
    }
};

typedef QList<QSharedPointer<ExportPlugin>> ExportPluginList;

Q_DECLARE_INTERFACE(ExportPlugin, ExporterInterface_iid)

#endif // PLUGIN_EXPORTER_HPP
