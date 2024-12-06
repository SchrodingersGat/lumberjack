#include "lumberjack_csv_importer.hpp"



LumberjackCSVImporter::LumberjackCSVImporter()
{

}


QStringList LumberjackCSVImporter::supportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes << "csv";
    fileTypes << "tsv";

    return fileTypes;
}
