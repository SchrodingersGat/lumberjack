#ifndef PLUGIN_IMPORTER_HPP
#define PLUGIN_IMPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

#define ImporterInterface_iid "org.lumberjack.plugins.ImportPlugin/1.0"


/**
 * @brief The ImportPlugin class defines an interface for importing data
 */
class ImportPlugin : public PluginBase
{
    Q_OBJECT
public:
    virtual ~ImportPlugin() = default;

    // Generate a new ImportPlugin instance
    virtual ImportPlugin* newInstance(void) const = 0;

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

    bool supportsFileType(QString fileType) const;
};

typedef QList<QSharedPointer<ImportPlugin>> ImportPluginList;

Q_DECLARE_INTERFACE(ImportPlugin, ImporterInterface_iid)

#endif // PLUGIN_IMPORTER_HPP
