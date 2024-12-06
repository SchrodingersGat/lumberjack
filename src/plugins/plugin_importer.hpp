#ifndef PLUGIN_IMPORTER_HPP
#define PLUGIN_IMPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"


class ImporterInterface : public PluginBase
{
public:
    virtual ~ImporterInterface() = default;
};


#define ImporterInterface_iid "org.lumberjack.plugins.ImporterInterface/1.0"

Q_DECLARE_INTERFACE(ImporterInterface, ImporterInterface_iid)

#endif // PLUGIN_IMPORTER_HPP
