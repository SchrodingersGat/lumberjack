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


/**
 * @brief LumberjackCSVImporter::loadDataFile
 * @param filename - The filename to load
 * @param errors -
 * @return
 */
bool LumberjackCSVImporter::loadDataFile(const QString &filename, QStringList &errors)
{
    // TODO: Implement this!
    return false;
}
