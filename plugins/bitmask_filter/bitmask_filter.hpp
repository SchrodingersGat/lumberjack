#ifndef LUMBERJACK_CSV_IMPORTER_HPP
#define LUMBERJACK_CSV_IMPORTER_HPP

#include "bitmask_filter_global.h"
#include "plugin_filter.hpp"


/**
 * TODO:
 * - Apply different bitmask operations (XOR / AND / OR)
 * - Specify bitmask
 */


/**
 * @brief The ScaleOffsetFilter class provides simple scaling and offset functionality
 */
class CUSTOM_FILTER_EXPORT BitmaskFilter : public FilterPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID FilterInterface_iid)
    Q_INTERFACES(FilterPlugin)
public:
    virtual ~BitmaskFilter() = default;

    // PluginBase functionality
    virtual QString pluginName(void) const override { return m_name; }
    virtual QString pluginDescription(void) const override { return m_description; }
    virtual QString pluginVersion(void) const override { return m_version; }

    // DataProcessingPlugin functionality
    virtual bool beforeProcessStep(void) override;
    virtual void afterProcessStep(void) override;
    virtual void cancelProcessing(void) override;
    virtual uint8_t getProgress(void) const override;

    // FilterPlugin functionality
    virtual unsigned int getMinInputCount(void) const override { return 1; }
    virtual unsigned int getMaxInputCount(void) const override { return 1; }
    virtual bool setFilterInputs(QList<DataSeriesPointer> inputs, QStringList &errors) override;
    virtual bool processData(void) override;
    virtual QList<DataSeriesPointer> getFilterOutputs(void) override;

protected:
    const QString m_name = "Bitmask Filter";
    const QString m_description = "Apply custom bitmasking patterns";
    const QString m_version = "0.1.0";

    uint32_t m_mask = 0x01234567;

    bool m_processing = false;

    DataSeriesPointer m_input;
    DataSeriesPointer m_output;
};

#endif // LUMBERJACK_CSV_IMPORTER_HPP
