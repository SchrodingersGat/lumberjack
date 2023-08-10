/**
 * Data importer plugin for delimited files such as CSV / TSV
 */


#ifndef CSV_IMPORTER_HPP
#define CSV_IMPORTER_HPP

#include <qdialog.h>

#include "data_source.hpp"

#include "ui_csv_import_options.h"


class CSVImportOptions
{
public:
    CSVImportOptions();
    ~CSVImportOptions();

    enum TimestampFormat {
        SECONDS = 0,
        MILLISECONDS = 1,
        HHMMSS = 2,
    };

    enum DelimiterType {
        COMMA,
        TAB,
        COLON,
        SEMICOLON,
        PIPE,
        SPACE,
    };

    bool zeroInitialTimestamp = false;
    int timestampColumn = 0;
    int headerRow = 0;
    int unitsRow = -1;
    TimestampFormat timestampFormat = TimestampFormat::SECONDS;
    DelimiterType delimiter = DelimiterType::COMMA;

    QString getDelimiter() const;
    double getTimestampScaler() const;

};


class CSVImportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    CSVImportOptionsDialog(QString filename, QWidget *parent = nullptr);
    ~CSVImportOptionsDialog();

    CSVImportOptions options;

public slots:
    void importData();

protected:
    Ui::csvImportOptionsDialog ui;

    QString filename;

    // Preview of the first lines of the file
    QStringList fileHead;

    bool getFilePreview();

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
    CSVImportOptions importOptions;

    QStringList headers;

    bool processRow(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractHeaders(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractData(int rowIndex, const QStringList &row, QStringList &errors);
    bool extractTimestamp(int rowIndex, const QStringList &row, double &timestamp);
    // Keep track of data columns while loading
    QHash<QString, QSharedPointer<DataSeries>> columnMap;
};


#endif // CSV_IMPORTER_HPP
