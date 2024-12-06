#ifndef PLUGIN_IMPORTER_HPP
#define PLUGIN_IMPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

class ImporterInterface : public PluginBase
{
    Q_OBJECT
public:
    virtual ~ImporterInterface() = default;

    // Return a list of support file types, e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    // Load data from the provided filename
    virtual bool loadDataFile(const QString &filename, QStringList &errors) = 0;

    // Return the loaded data
    // virtual DataSource data(void) const;
};


#define ImporterInterface_iid "org.lumberjack.plugins.ImporterInterface/1.0"

Q_DECLARE_INTERFACE(ImporterInterface, ImporterInterface_iid)

#endif // PLUGIN_IMPORTER_HPP
