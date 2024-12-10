#include "lumberjack_csv_exporter.hpp"



LumberjackCSVExporter::LumberjackCSVExporter()
{

}


QStringList LumberjackCSVExporter::supportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes << "csv";
    fileTypes << "tsv";

    return fileTypes;
}

