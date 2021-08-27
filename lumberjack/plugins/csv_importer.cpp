#include "csv_importer.hpp"


CSVImporter::CSVImporter() : FileDataSource("CSV Importer")
{

}


CSVImporter::~CSVImporter()
{
    // TODO - Any custom cleanup?
}


QStringList CSVImporter::getSupportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes.append("csv");
    fileTypes.append("tsv");
    fileTypes.append("tab");

    return fileTypes;
}


bool CSVImporter::loadDataFromFile(QStringList &errors)
{
    // TODO

    return false;
}
