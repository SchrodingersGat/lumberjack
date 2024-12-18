#ifndef PLUGIN_BASE_HPP
#define PLUGIN_BASE_HPP

#include <QObject>
#include <QString>

/**
 * @brief The PluginBase class forms the basis of any Lumberjack plugin
 */
class PluginBase : public QObject
{
    Q_OBJECT
public:
    virtual ~PluginBase() = default;

    // Return the name of this plugin
    virtual QString pluginName(void) const = 0;

    // Return the description of this plugin
    virtual QString pluginDescription(void) const = 0;

    // Return the version of this plugin
    virtual QString pluginVersion(void) const= 0;

    // Return the IID string associated with this plugin
    virtual QString pluginIID(void) const = 0;
};

typedef QList<QSharedPointer<PluginBase>> PluginList;


// Base plugin class which provides data processing functionality
class DataProcessingPlugin : public PluginBase
{
    Q_OBJECT

public:
    virtual ~DataProcessingPlugin() = default;

    // Optional function which is called before running data processing step
    // Return False to cancel the process before it begins
    virtual bool beforeProcessStep(void) { return true; }

    // Optional function which is called after running the data processing step
    virtual void afterProcessStep(void) {}

    // Function to cancel the process step
    // MUST be implemented by the particular plugin class
    virtual void cancelProcessing(void) = 0;

    // Return the progress of the data processing stage (as a percentage {0:100})
    virtual uint8_t getProgress(void) const = 0;

};

#endif // PLUGIN_BASE_HPP
