#include <QFile>

#include "csv_importer.h"

CSV_Importer::CSV_Importer() : DataSource("")
{
    // TODO
}


QStringList CSV_Importer::getSupportedFileTypes()
{
    QStringList types;

    types.append("csv");
    types.append("tsv");
    types.append("tab");

    return types;
}


bool CSV_Importer::loadFromFile(const QString filename)
{
    QFile csv_file(filename);

    // Initially check that the file exists!
    if (!csv_file.exists()) return false;

    // Can we open the file?
    if (!csv_file.open(QIODevice::ReadOnly) || !csv_file.isOpen())
    {
        return false;
    }

    // TODO?

    // Ensure the file is closed
    csv_file.close();

    return true;
}
