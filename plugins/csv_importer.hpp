/**
 * Data importer plugin for delimited files such as CSV / TSV
 */


#ifndef CSV_IMPORTER_HPP
#define CSV_IMPORTER_HPP

#include <qdialog.h>

#include "data_source.hpp"

#include "ui_csv_import_options.h"


class CSVImportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    CSVImportOptionsDialog(QString filename, QWidget *parent = nullptr);
    ~CSVImportOptionsDialog();

protected:
    Ui::csvImportOptionsDialog ui;

    QString filename;
};



class CSVImporter : public FileDataSource
{
    Q_OBJECT

public:
    CSVImporter();
    virtual ~CSVImporter();

    virtual QString getFileDescription(void) const override { return "CSV Files"; }
    virtual QStringList getSupportedFileTypes(void) const override;

    virtual bool setImportOptions() override;
    virtual bool loadDataFromFile(QStringList& errors) override;

protected:
    // Import configuration options
    int timestampColumn = 0;
    int headerRow = 0;
    double timestampScaler = 0.001;     // Convert from ms to s

    QString delimiter = ",";

    QStringList headers;

    bool processRow(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractHeaders(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractData(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractTimestamp(int rowIndex, const QStringList &row, double &timestamp);
    // Keep track of data columns while loading
    QHash<QString, QSharedPointer<DataSeries>> columnMap;
};


#endif // CSV_IMPORTER_HPP
