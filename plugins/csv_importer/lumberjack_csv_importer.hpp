#ifndef LUMBERJACK_CSV_IMPORTER_HPP
#define LUMBERJACK_CSV_IMPORTER_HPP

#include <QFile>

#include "plugin_importer.hpp"
#include "csv_import_options.hpp"


class LumberjackCSVImporter : public ImportPlugin
{
    Q_OBJECT
public:
    LumberjackCSVImporter();

    // Base plugin functionality
    virtual QString pluginName(void) const override { return m_name; }
    virtual QString pluginDescription(void) const override { return m_description; }
    virtual QString pluginVersion(void) const override { return m_version; }

    // Importer plugin functionality
    virtual QStringList supportedFileTypes(void) const override;

    virtual bool beforeImport(void) override;
    virtual bool importData(QStringList &errors) override;
    virtual void afterImport(void) override;
    virtual void cancelImport(void) override;

    virtual uint8_t getImportProgress(void) const override;

    virtual QList<QSharedPointer<DataSeries>> getDataSeries(void) const override;

protected:
    //! Plugin metadata
    const QString m_name = "CSV Importer";
    const QString m_description = "Import data from CSV files";
    const QString m_version = "0.1.0";

    //! CSV import options
    CSVImportOptions m_options;

    QStringList m_headers;

    //! Internal functions for processing data
    bool processRow(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractHeaders(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractData(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractTimestamp(int rowIndex, const QStringList &row, double &timestamp);

    // Keep track of data columns while loading
    QHash<QString, QSharedPointer<DataSeries>> columnMap;

    // Keep track of first timestamp value
    double initialTimetamp = 0;
    bool initialTimestampSeen = false;

    // Internal timestamp which is used if not available in imported file
    double incrementingTimestamp = 0;

    //! File object being imported
    QFile *m_file = nullptr;

    bool m_isImporting = false;

    //! Number of bytes processed from the file
    int64_t m_bytesRead;

    //! Total number of bytes in the file
    int64_t m_fileSize;

};

#endif // LUMBERJACK_CSV_IMPORTER_HPP
