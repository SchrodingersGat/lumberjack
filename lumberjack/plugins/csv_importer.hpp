/**
 * Data importer plugin for delimited files such as CSV / TSV
 */


#ifndef CSV_IMPORTER_HPP
#define CSV_IMPORTER_HPP

#include "data_source.hpp"


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

    QString delimiter = ",";

    QStringList headers;

    bool processRow(int rowIndex, QStringList row, QStringList &errors);
    bool extractHeaders(int rowIndex, QStringList row, QStringList &errors);
    bool extractData(int rowIndex, QStringList row, QStringList &errors);
};


#endif // CSV_IMPORTER_HPP
