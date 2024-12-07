#ifndef PLUGIN_IMPORTER_HPP
#define PLUGIN_IMPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

#define ImporterInterface_iid "org.lumberjack.plugins.ImporterInterface/1.0"

namespace Lumberjack {

class ImporterPlugin : public PluginBase
{
    Q_OBJECT
public:
    virtual ~ImporterPlugin() = default;

    // Generate a new ImporterInterface instance
    virtual ImporterPlugin* newInstance(void) const = 0;

    // Return a list of support file types, e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    // Load data from the provided filename
    virtual bool loadDataFile(const QString &filename, QStringList &errors) = 0;

    // Return the loaded data
    // virtual DataSource data(void) const;

    // Return the IID string
    virtual QString pluginIID(void) const override
    {
        return QString(ImporterInterface_iid);
    }
};

};

Q_DECLARE_INTERFACE(Lumberjack::ImporterPlugin, ImporterInterface_iid)

#endif // PLUGIN_IMPORTER_HPP
