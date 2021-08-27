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
    virtual bool loadDataFromFile(QStringList& errors) override;
};


#endif // CSV_IMPORTER_HPP
