#ifndef PLUGIN_FILTER_HPP
#define PLUGIN_FILTER_HPP

#include <QtPlugin>

#include "data_series.hpp"
#include "plugin_base.hpp"

#define FilterInterface_iid "org.lumberjack.plugins.FilterPlugin/1.0"



/**
 * @brief The FilterPlugin class defines an interface for applying custom data filters
 */
class FilterPlugin : public DataProcessingPlugin
{
    Q_OBJECT
public:
    virtual ~FilterPlugin() = default;

    // Return the IID string
    virtual QString pluginIID(void) const override
    {
        return QString(FilterInterface_iid);
    }

    // Return the minimum number of inputs supported by this plugin
    virtual unsigned int getMinInputCount(void) const = 0;

    // Return the maximum number of inputs supported by this plugin
    virtual unsigned int getMaxInputCount(void) const = 0;

    // Set the filter inputs
    virtual bool setFilterInputs(QList<DataSeriesPointer> inputs, QStringList &errors);

    // Return the generated data series output(s)
    virtual QList<DataSeriesPointer> getFilterOutputs(void) = 0;

protected:
    QList<DataSeriesPointer> m_inputs;
};

typedef QList<QSharedPointer<FilterPlugin>> FilterPluginList;

Q_DECLARE_INTERFACE(FilterPlugin, FilterInterface_iid)

#endif // PLUGIN_FILTER_HPP
