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


bool LumberjackCSVExporter::beforeProcessStep(void)
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

    // Copy across data series
    m_data.clear();
    m_indices.clear();

    for (auto s : series)
    {
        if (!s.isNull())
        {
            m_data.append(s);
            m_indices.append(0);
        }
    }

    QStringList row;

    // Write header row
    row = headerRow();
    outputFile.write(rowToString(row));

    if (m_unitsRow)
    {
        row = unitsRow();
        outputFile.write(rowToString(row));
    }

    // Extract min/max timestamps (for progress bar)
    double tMin = LONG_MAX;
    double tMax = -LONG_MAX;

    for (auto s : series)
    {
        if (s.isNull()) continue;
        if (s->size() == 0) continue;

        DataPoint p1 = s->getDataPoint(0);
        DataPoint p2 = s->getDataPoint(s->size() - 1);

        if (p1.timestamp < tMin) tMin = p1.timestamp;
        if (p2.timestamp > tMax) tMax = p2.timestamp;
    }

    m_minTimestamp = tMin;
    m_maxTimestamp = tMax;

    bool valid = true;

    m_isExporting = true;

    while (valid && m_isExporting)
    {
        row = nextDataRow(valid);

        if (valid)
        {
            outputFile.write(rowToString(row));
        }
    }

    outputFile.close();

    return true;
}


QByteArray LumberjackCSVExporter::rowToString(QStringList &row) const
{
    QString data = row.join(m_delimiter).trimmed() + "\n";

    return data.toLatin1();
}


/**
 * @brief LumberjackCSVExporter::headerRow - Generate a header row for the exported dataset
 * @return
 */
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


/**
 * @brief LumberjackCSVExporter::unitsRow - Generate a units row for the exported dataset
 * @return
 */
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


/**
 * @brief LumberjackCSVExporter::nextDataRow - Generate the next row of data
 * @param valid
 * @return
 */
QStringList LumberjackCSVExporter::nextDataRow(bool &valid)
{
    // TODO: Better configuration of timestamp resolution...
    const double DT = 1e-6;

    QStringList values;

    double nextTimestamp = LONG_MAX;

    int N = qMin(m_data.count(), m_indices.count());

    bool dataAvailable = false;

    // What is the *smallest* timestamp?
    for (int ii = 0; ii < N; ii++)
    {
        auto series = m_data.at(ii);
        uint64_t idx = m_indices.at(ii);

        if (idx < series->size())
        {
            dataAvailable = true;

            auto point = series->getDataPoint(idx);

            if (point.timestamp < nextTimestamp)
            {
                nextTimestamp = point.timestamp;
            }
        }
    }

    m_currentTimestamp = nextTimestamp;

    // No more data available
    if (!dataAvailable)
    {
        valid = false;
        return values;
    }

    // Timestamp
    values.append(QString::number(nextTimestamp));

    // Now, iterate through each series, and see if it has a corresponding value at this timestamp
    for (int ii = 0; ii < N; ii++)
    {
        auto series = m_data.at(ii);
        uint64_t idx = m_indices.at(ii);

        QString value;

        if (idx < series->size())
        {
            auto point = series->getDataPoint(idx);

            // Timestamp is within allowable range
            if (point.timestamp <= (nextTimestamp + DT))
            {
                value = QString::number(point.value);
                m_indices[ii]++;
            }
        }

        // TODO: "Empty" value compensation?
        values.append(value);
    }

    valid = true;
    return values;
}


void LumberjackCSVExporter::cancelProcessing()
{
    m_isExporting = false;
}


uint8_t LumberjackCSVExporter::getProgress(void) const
{
    double dt = m_maxTimestamp - m_minTimestamp;

    if (dt == 0) return 0;

    double progress = (m_currentTimestamp - m_minTimestamp) / dt;

    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;

    return (uint8_t) (progress * 100);
}
