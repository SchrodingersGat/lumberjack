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


bool LumberjackCSVExporter::exportData(QList<DataSeriesPointer> &series)
{
    // TODO - Implement me!

    qDebug() << "Export data to: " << m_filename;

    for (auto s : series)
    {
        if (s.isNull()) continue;

        qDebug() << "-" << s->getLabel();
    }

    return true;
}
