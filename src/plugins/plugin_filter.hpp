#ifndef PLUGIN_FILTER_HPP
#define PLUGIN_FILTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

#define FilterInterface_iid "org.lumberjack.plugins.FilterPlugin/1.0"



/**
 * @brief The FilterPlugin class defines an interface for applying custom data filters
 */
class FilterPlugin : public PluginBase
{
    Q_OBJECT
public:
    virtual ~FilterPlugin() = default;

    // Return the IID string
    virtual QString pluginIID(void) const override
    {
        return QString(FilterInterface_iid);
    }


    // TODO
};

typedef QList<QSharedPointer<FilterPlugin>> FilterPluginList;

Q_DECLARE_INTERFACE(FilterPlugin, FilterInterface_iid)

#endif // PLUGIN_FILTER_HPP
