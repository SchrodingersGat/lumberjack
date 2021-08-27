#include "cedat_importer.hpp"


CEDATImporter::CEDATImporter() : FileDataSource("CEDAT Importer")
{

}


CEDATImporter::~CEDATImporter()
{
    // TODO - Any custom cleanup?
}


QStringList CEDATImporter::getSupportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes.append("cedat");

    return fileTypes;
}


bool CEDATImporter::loadDataFromFile(QStringList &errors)
{
    // TODO

    return false;
}
