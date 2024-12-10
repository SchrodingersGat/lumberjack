#ifndef LUMBERJACK_CSV_IMPORTER_HPP
#define LUMBERJACK_CSV_IMPORTER_HPP

#include "scaler_filter_global.h"
#include "plugin_filter.hpp"


/**
 * @brief The ScaleOffsetFilter class provides simple scaling and offset functionality
 */
class CUSTOM_FILTER_EXPORT ScalerFilter : public FilterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID FilterInterface_iid)
    Q_INTERFACES(FilterPlugin)
public:
    ScalerFilter();

    // Base plugin functionality
    virtual QString pluginName(void) const override { return m_name; }
    virtual QString pluginDescription(void) const override { return m_description; }
    virtual QString pluginVersion(void) const override { return m_version; }

protected:
    const QString m_name = "Scaler Filter";
    const QString m_description = "Apply custom scaler to a dataset";
    const QString m_version = "0.1.0";

};

#endif // LUMBERJACK_CSV_IMPORTER_HPP
