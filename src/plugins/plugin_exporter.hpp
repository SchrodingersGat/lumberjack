#ifndef PLUGIN_EXPORTER_HPP
#define PLUGIN_EXPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

#define ExporterInterface_iid "org.lumberjack.plugins.ExporterInterface/1.0"

/**
 * @brief The ExporterPlugin class defines an interface for exporting data
 */
class ExporterPlugin : public PluginBase
{
    Q_OBJECT
public:
    virtual ~ExporterPlugin() = default;

    // Generate a new ExporterPlugin interface
    virtual ExporterPlugin* newInstance(void) const = 0;

    // Return a list of the support file types e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    // TODO: Export data function

    virtual QString pluginIID(void) const override
    {
        return QString(ExporterInterface_iid);
    }
};

typedef QList<QSharedPointer<ExporterPlugin>> ExporterPluginList;

Q_DECLARE_INTERFACE(ExporterPlugin, ExporterInterface_iid)

#endif // PLUGIN_EXPORTER_HPP
