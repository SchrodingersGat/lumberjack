#ifndef PLUGIN_EXPORTER_HPP
#define PLUGIN_EXPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"
#include "data_series.hpp"

#define ExporterInterface_iid "org.lumberjack.plugins.ExportPlugin/1.0"

/**
 * @brief The ExportPlugin class defines an interface for exporting data
 */
class ExportPlugin : public PluginBase
{
    Q_OBJECT
public:
    virtual ~ExportPlugin() = default;

    // Return a list of the support file types e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    // Optional function called before data export
    // Return False to cancel the data export process
    virtual bool beforeExport(void) { return true; }

    // Export data to the provided filename
    virtual bool exportData(QList<DataSeriesPointer> &series, QStringList &errors) = 0;

    // Optional function called after data export
    virtual void afterExport(void) {}

    // Cancel import process - plugin is expected to perform cleanup
    virtual void cancelExport(void) = 0;

    // Return the progress of the data export process (as a percentage {0:100})
    virtual uint8_t getExportProgress(void) const = 0;

    virtual QString pluginIID(void) const override
    {
        return QString(ExporterInterface_iid);
    }

    QString fileFilter(void) const;

    bool supportsFileType(QString fileType) const;

    void setFilename(QString filename) { m_filename = filename; }
    QString getFilename(void) const { return m_filename; }

protected:
    // Stored filename, destination of exported data
    QString m_filename;
};

typedef QList<QSharedPointer<ExportPlugin>> ExportPluginList;

Q_DECLARE_INTERFACE(ExportPlugin, ExporterInterface_iid)

#endif // PLUGIN_EXPORTER_HPP
