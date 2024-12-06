#ifndef PLUGIN_BASE_HPP
#define PLUGIN_BASE_HPP

#include <QString>


/**
 * @brief The PluginBase class forms the basis of any Lumberjack plugin
 */
class PluginBase
{
public:
    virtual ~PluginBase() = default;

    // Plugin information getters
    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;
};


#endif // PLUGIN_BASE_HPP
