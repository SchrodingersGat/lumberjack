#ifndef LUMBERJACK_OFFSET_FILTER_HPP
#define LUMBERJACK_OFFSET_FILTER_HPP

#include "plugin_filter.hpp"


/**
 * @brief The ScaleOffsetFilter class provides simple scaling and offset functionality
 */
class OffsetFilter : public FilterPlugin
{
    Q_OBJECT
public:
    OffsetFilter();

    // Base plugin functionality
    virtual QString pluginName(void) const override { return m_name; }
    virtual QString pluginDescription(void) const override { return m_description; }
    virtual QString pluginVersion(void) const override { return m_version; }

protected:
    const QString m_name = "Offset Filter";
    const QString m_description = "Apply custom offset to a dataset";
    const QString m_version = "0.1.0";

};

#endif // LUMBERJACK_OFFSET_FILTER_HPP
