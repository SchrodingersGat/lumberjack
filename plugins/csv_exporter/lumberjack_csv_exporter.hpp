#ifndef LUMBERJACK_CSV_EXPORTER_HPP
#define LUMBERJACK_CSV_EXPORTER_HPP

#include "plugin_exporter.hpp"


class LumberjackCSVExporter : public ExportPlugin
{
    Q_OBJECT

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
    virtual void cancelExport(void) override;
    virtual uint8_t getExportProgress(void) const override;

protected:
    const QString m_name = "CSV Exporter";
    const QString m_description = "Export data to CSV file";
    const QString m_version = "0.1.0";

    QList<DataSeriesPointer> m_data;
    QList<uint64_t> m_indices;

    bool m_isExporting = false;

    double m_currentTimestamp = 0;
    double m_minTimestamp = 0;
    double m_maxTimestamp = 0;

    // Data export options
    QString m_delimiter = ",";

    bool m_zeroTimestamp = false;
    bool m_unitsRow = false;

    // Internal helper functions
    QByteArray rowToString(QStringList &row) const;
    QStringList headerRow(void) const;
    QStringList unitsRow(void) const;

    QStringList nextDataRow(bool &valid);

};

#endif // LUMBERJACK_CSV_EXPORTER_HPP
