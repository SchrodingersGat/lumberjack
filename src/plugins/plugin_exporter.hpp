#ifndef PLUGIN_EXPORTER_HPP
#define PLUGIN_EXPORTER_HPP

#include <QtPlugin>

#include "plugin_base.hpp"
#include "data_series.hpp"

#define ExporterInterface_iid "org.lumberjack.plugins.ExportPlugin/1.0"

/**
 * @brief The ExportPlugin class defines an interface for exporting data
 */
class ExportPlugin : public DataProcessingPlugin
{
    Q_OBJECT
public:
    virtual ~ExportPlugin() = default;

    // Return a list of the support file types e.g. ['csv', 'tsv']
    virtual QStringList supportedFileTypes(void) const = 0;

    virtual QString pluginIID(void) const override
    {
        return QString(ExporterInterface_iid);
    }

    QString fileFilter(void) const;

    bool supportsFileType(QString fileType) const;

    virtual void setFilename(QString filename) { m_filename = filename; }
    virtual void setDataSeries(QList<DataSeriesPointer> seriesList) { m_series = seriesList; }
    QString getFilename(void) const { return m_filename; }

protected:
    // Stored filename, destination of exported data
    QString m_filename;

    QList<DataSeriesPointer> m_series;
};

typedef QList<QSharedPointer<ExportPlugin>> ExportPluginList;

Q_DECLARE_INTERFACE(ExportPlugin, ExporterInterface_iid)

#endif // PLUGIN_EXPORTER_HPP
