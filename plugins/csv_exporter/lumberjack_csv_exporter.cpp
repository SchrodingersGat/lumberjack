#include <QFile>

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


bool LumberjackCSVExporter::beforeExport(void)
{
    // TODO: Set export options
    return true;
}


/*
 * Export the provided series to a CSV file
 */
bool LumberjackCSVExporter::exportData(QList<DataSeriesPointer> &series, QStringList &errors)
{
   if (m_filename.isEmpty())
    {
        errors.append(tr("Filename is empty"));
        return false;
    }

    QFile outputFile(m_filename);

    if (!outputFile.open(QIODevice::WriteOnly) || !outputFile.isOpen() || !outputFile.isWritable())
    {
        errors.append(tr("Could not open file for writing"));
        return false;
    }

    m_data = series;

    QStringList row;

    // Write header row
    row = headerRow();
    outputFile.write(rowToString(row));

    if (m_unitsRow)
    {
        row = unitsRow();
        outputFile.write(rowToString(row));
    }

    outputFile.close();

    // TODO: Generate each data row

    return true;
}


QByteArray LumberjackCSVExporter::rowToString(QStringList &row) const
{
    QString data = row.join(m_delimiter).trimmed() + "\n";

    return data.toLatin1();
}


QStringList LumberjackCSVExporter::headerRow(void) const
{
    QStringList header;

    header << tr("Timestamp");

    for (auto series : m_data)
    {
        if (series.isNull()) continue;

        header.append(series->getLabel());
    }

    return header;
}


QStringList LumberjackCSVExporter::unitsRow(void) const
{
    QStringList units;

    units << QString("");

    for (auto series : m_data)
    {
        if (series.isNull()) continue;

        units.append(series->getUnits());
    }

    return units;
}

