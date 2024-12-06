#ifndef PLUGIN_BASE_HPP
#define PLUGIN_BASE_HPP

#include <QObject>
#include <QString>

namespace Lumberjack {

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
};

}

#endif // PLUGIN_BASE_HPP
