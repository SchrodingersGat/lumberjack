#include <QFile>
#include <QFileInfo>

#include <math.h>

#include <QDialog>

#include "lumberjack_csv_importer.hpp"
#include "import_options_dialog.hpp"


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
 * @brief LumberjackCSVImporter::beforeLoadData - Open configuration dialog
 * @return
 */
bool LumberjackCSVImporter::beforeProcessStep(void)
{
    CSVImportOptionsDialog dlg(m_filename);

    int result = dlg.exec();

    if (result == QDialog::Accepted)
    {
        m_options = dlg.getOptions();
    }

    return result == QDialog::Accepted;
}


/**
 * @brief LumberjackCSVImporter::processData
 * @param filename - The filename to load
 * @param errors -
 * @return
 */
bool LumberjackCSVImporter::processData(void)
{
    // Reset importer to initial conditions
    m_headers.clear();
    columnMap.clear();
    incrementingTimestamp = 0;
    initialTimestampSeen = false;

    QFileInfo fi(m_filename);

    if (!fi.exists() || !fi.isFile())
    {
        qCritical() << tr("File does not exist");
        return false;
    }

    m_bytesRead = 0;
    m_fileSize = fi.size();

    m_file = new QFile(m_filename);

    if (!m_file->open(QIODevice::ReadOnly) || !m_file->isOpen() || !m_file->isReadable())
    {
        qCritical() << tr("Could not open file for reading");
        m_file->close();
        return false;
    }

    QString delimiter = m_options.getDelimiterString();

    qint64 lineCount = 0;
    qint64 badLineCount = 0;

    // Ensure we are at the start of the file
    m_file->seek(0);

    QStringList row;

    m_isImporting = true;

    while (!m_file->atEnd() && m_isImporting && m_file != nullptr)
    {
        QByteArray bytes = m_file->readLine();

        QString line(bytes);

        line = line.trimmed();

        // Ignore lines which start with prohibited characters
        if (!m_options.ignoreRowsStartingWith.isEmpty() && line.startsWith(m_options.ignoreRowsStartingWith)) {
            continue;
        }

        row = line.split(delimiter);

        if (!processRow(lineCount, row))
        {
            badLineCount++;
        }

        m_bytesRead += line.length();

        lineCount++;
    }

    // Ensure file object is closed
    m_file->close();

    m_isImporting = false;

    if (badLineCount > 0)
    {
        qWarning() << QString("Lines with errors: " + QString::number(badLineCount));
    }

    return true;
}


/**
 * @brief LumberjackCSVImporter::processRow - Process a single row of data from the file
 * @param rowIndex - The row index with in the file
 * @param row - Delimited row data
 * @param errors
 * @return
 */
bool LumberjackCSVImporter::processRow(int rowIndex, const QStringList &row)
{

    if (rowIndex == m_options.rowHeaders)
    {
        return extractHeaders(rowIndex, row);
    }
    else if (rowIndex == m_options.rowUnits)
    {
        // TODO - Extract units data

        return true;
    }
    else if (rowIndex >= m_options.rowDataStart)
    {
        return extractData(rowIndex, row);
    }
}


/**
 * @brief LumberjackCSVImporter::extractHeaders - Extract headers from the provided file
 * @param rowIndex
 * @param row
 * @param errors
 * @return
 */
bool LumberjackCSVImporter::extractHeaders(int rowIndex, const QStringList &row)
{
    Q_UNUSED(rowIndex);

    m_headers.clear();

    for (int ii = 0; ii < row.length(); ii++)
    {
        QString header = row.at(ii).trimmed();

        if (header.isEmpty())
        {
            qWarning() << "Empty header in column" << ii;

            // Default column headers
            header = "Column " + QString::number(ii);
        }

        m_headers.append(header);

        // Ignore timestamp column
        if (m_options.hasTimestamp && ii == m_options.colTimestamp)
        {
            continue;
        }

        // Check that we don't have a duplicate header already
        if (!columnMap.contains(header))
        {
            columnMap.insert(
                header,
                QSharedPointer<DataSeries>(new DataSeries(header))
            );
        }
    }

    return true;
}



/**
 * @brief LumberjackCSVImporter::extractData - Extract data from a given row
 * @param rowIndex
 * @param row
 * @param errors
 * @return
 */
bool LumberjackCSVImporter::extractData(int rowIndex, const QStringList &row)
{
    double timestamp = 0;

    QString text;
    double value = 0;
    bool result = false;

    if (!m_options.hasTimestamp)
    {
        timestamp = incrementingTimestamp + m_options.getTimestampScaler();
    }
    else if (!extractTimestamp(rowIndex, row, timestamp))
    {
        qWarning() << "CSV:" << "Line" << rowIndex << "does not contain valid timestamp";
        return false;
    }

    if (!initialTimestampSeen)
    {
        initialTimestampSeen = true;
        initialTimetamp = timestamp;
    }

    if (m_options.colTimestamp >= 0 && m_options.zeroTimestamp)
    {
        timestamp -= initialTimetamp;
    }

    for (int ii = 0; ii < row.length(); ii++)
    {
        // Ignore the timestamp column
        if (ii == m_options.colTimestamp)
        {
            continue;
        }

        QString text = row.at(ii).trimmed();

        if (ii >= m_headers.length())
        {
            qWarning() << "CSV: Line" << rowIndex << "exceeded header count";
            continue;
        }

        // Ignore empty cell values
        if (text.isEmpty())
        {
            continue;
        }

        text = text.toLower();

        if (text == "true" || text == "on" || text == "yes" || text == "y")
        {
            value = 1;
            result = true;
        }
        else if (text == "false" || text == "off" || text == "no" || text == "n")
        {
            value = 0;
            result = true;
        }
        else
        {
            value = text.toDouble(&result);
        }

        // Data could not be converted to a number
        if (!result) continue;

        // Ignore invalid or infinite values
        if (isnan(value) || isinf(value)) continue;

        QString header = m_headers.at(ii);
        QString backupHeader = "Column " + QString::number(ii);

        QSharedPointer<DataSeries> series;

        if (columnMap.contains(header))
        {
            series = columnMap.value(header);
        }
        else if (columnMap.contains(backupHeader))
        {
            series = columnMap.value(backupHeader);
        }

        if (series.isNull())
        {
            qWarning() << "CSV: No series matching label" << header;
        }
        else
        {
            series->addData(timestamp, value, false);
        }
    }

    return true;
}


/**
 * @brief LumberjackCSVImporter::extractTimestamp - Extract timestamp information from the provided row
 * @param rowIndex
 * @param row
 * @param timestamp
 * @return
 */
bool LumberjackCSVImporter::extractTimestamp(int rowIndex, const QStringList &row, double &timestamp)
{
    if (row.length() <= m_options.colTimestamp)
    {
        qWarning() << "Line" << rowIndex << "missing timestamp column";
        return false;
    }

    QString ts = row.at(m_options.colTimestamp);

    bool result = false;

    double timestampScaler = m_options.getTimestampScaler();

    // Convert from hh:mm::ss
    if (ts.contains(":"))
    {
        QStringList hhmmss = ts.trimmed().split(":");

        double t = 0;

        double hh = 0;
        double mm = 0;
        double ss = 0;
        double ms = 0;

        bool hhOk = true;
        bool mmOk = true;
        bool ssOk = true;
        bool msOk = true;

        // Expected format is hh:mm:ss:ms

        if (hhmmss.length() > 0)
        {
            hh = hhmmss[0].toDouble(&hhOk);
        }

        if (hhmmss.length() > 1)
        {
            mm = hhmmss[1].toDouble(&mmOk);
        }

        if (hhmmss.length() > 2)
        {
            ss = hhmmss[2].toDouble(&ssOk);
        }

        // Process milliseconds
        if (hhmmss.length() > 3)
        {
            ms = hhmmss[3].toDouble(&msOk);

            if (msOk)
            {
                // "Resolution" of milliseconds depends on the length of the provided string
                ms /= pow(10, hhmmss[3].length());
            }
        }

        if (hhOk && mmOk && ssOk && msOk)
        {
            t = ss;
            t += (mm * 60);
            t += (hh * 3600);
            t += ms;

            timestamp = t;

            return true;
        }

        return false;

    }

    timestamp = ts.toDouble(&result);

    if (result)
    {
        timestamp *= timestampScaler;
    }

    return result;
}


void LumberjackCSVImporter::afterProcessStep(void)
{
    if (m_file)
    {
        if (m_file->isOpen())
        {
            m_file->close();
        }

        delete m_file;
        m_file = nullptr;
    }
}


void LumberjackCSVImporter::cancelProcessing(void)
{
    m_isImporting = false;
}


uint8_t LumberjackCSVImporter::getProgress(void) const
{
    if (!m_isImporting) return 0;

    if (m_bytesRead == 0 || m_fileSize == 0) return 0;

    float progress = (float) m_bytesRead / (float) m_fileSize;

    return (uint8_t) (progress * 100);
}


/**
 * Return the list of imported data series
 */
QList<QSharedPointer<DataSeries>> LumberjackCSVImporter::getDataSeries(void) const
{
    return columnMap.values();
}
