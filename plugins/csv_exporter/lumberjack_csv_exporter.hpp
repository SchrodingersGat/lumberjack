#ifndef LUMBERJACK_CSV_IMPORTER_HPP
#define LUMBERJACK_CSV_IMPORTER_HPP

#include "csv_exporter_global.h"
#include "plugin_exporter.hpp"


class CSV_EXPORTER_EXPORT LumberjackCSVExporter : public ExportPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ExporterInterface_iid)
    Q_INTERFACES(ExportPlugin)
public:
    LumberjackCSVExporter();

    // Base plugin functionality
    virtual QString pluginName(void) const override { return m_name; }
    virtual QString pluginDescription(void) const override { return m_description; }
    virtual QString pluginVersion(void) const override { return m_version; }

    // Exporter plugin functionality
    virtual QStringList supportedFileTypes(void) const override;

    virtual bool beforeExport(void) override;
    virtual bool exportData(QList<DataSeriesPointer> &series, QStringList &errors) override;

protected:
    const QString m_name = "CSV Exporter";
    const QString m_description = "Export data to CSV file";
    const QString m_version = "0.1.0";

    QList<DataSeriesPointer> m_data;

    // Data export options
    QString m_delimiter = ",";

    bool m_zeroTimestamp = false;
    bool m_unitsRow = false;

    // Internal helper functions
    QByteArray rowToString(QStringList &row) const;
    QStringList headerRow(void) const;
    QStringList unitsRow(void) const;

};

#endif // LUMBERJACK_CSV_IMPORTER_HPP
