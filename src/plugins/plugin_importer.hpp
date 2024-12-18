#ifndef PLUGIN_IMPORTER_HPP
#define PLUGIN_IMPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"

#include "data_series.hpp"

#define ImporterInterface_iid "org.lumberjack.plugins.ImportPlugin/1.0"


/**
 * @brief The ImportPlugin class defines an interface for importing data
 */
class ImportPlugin : public DataProcessingPlugin
{
    Q_OBJECT
public:
    virtual ~ImportPlugin() = default;

    // Return a list of support file types, e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    // Validate file before opening
    // Note: The default implementation performs basic functionality,
    //       but this can be extended by the plugin
    virtual bool validateFile(QString filename, QStringList &errors) const;

    // After import, plugin must return a list of DataSeries objects
    virtual QList<DataSeriesPointer> getDataSeries(void) const = 0;

    // Return the IID string
    virtual QString pluginIID(void) const override
    {
        return QString(ImporterInterface_iid);
    }

    QString fileFilter(void) const;

    bool supportsFileType(QString fileType) const;

    void setFilename(QString filename) { m_filename = filename; }
    QString getFilename(void) const { return m_filename; }

protected:
    // Stored filename, source of imported data
    QString m_filename;
};

typedef QList<QSharedPointer<ImportPlugin>> ImportPluginList;

Q_DECLARE_INTERFACE(ImportPlugin, ImporterInterface_iid)

#endif // PLUGIN_IMPORTER_HPP
