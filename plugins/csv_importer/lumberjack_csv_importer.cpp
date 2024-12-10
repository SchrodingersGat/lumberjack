#include <QFile>
#include <QFileInfo>

#include <math.h>

#include <QDialog>
#include <QProgressDialog>

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


QString LumberjackCSVImporter::getDelimiterCharacter(void) const
{
    switch (m_options.delimeter)
    {
    default:
    case CSVImportOptions::DelimiterType::COMMA:
        return ",";
    case CSVImportOptions::DelimiterType::TAB:
        return "\t";
    case CSVImportOptions::DelimiterType::COLON:
        return ":";
    case CSVImportOptions::DelimiterType::SEMICOLON:
        return ";";
    case CSVImportOptions::DelimiterType::PIPE:
        return "|";
    case CSVImportOptions::DelimiterType::SPACE:
        return " ";
    }
}


double LumberjackCSVImporter::getTimestampScaler(void) const
{
    switch (m_options.timestampFormat)
    {
    default:
    case CSVImportOptions::TimestampFormat::SECONDS:
    case CSVImportOptions::TimestampFormat::HHMMSS:
        return 1.0;
    case CSVImportOptions::TimestampFormat::MILLISECONDS:
        return 0.001;
    }
}


/**
 * @brief LumberjackCSVImporter::beforeLoadData - Open configuration dialog
 * @return
 */
bool LumberjackCSVImporter::beforeLoadData(void)
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
 * @brief LumberjackCSVImporter::loadDataFile
 * @param filename - The filename to load
 * @param errors -
 * @return
 */
bool LumberjackCSVImporter::loadDataFile(QStringList &errors)
{
    // Reset importer to initial conditions
    m_headers.clear();
    columnMap.clear();
    incrementingTimestamp = 0;
    initialTimestampSeen = false;

    QFile f(m_filename);
    QFileInfo fi(m_filename);

    qint64 fileSize = fi.size();

    // Open the file
    if (!f.exists() || !f.open(QIODevice::ReadOnly) || !f.isOpen() || !f.isReadable())
    {
        errors.append(tr("Could not open file for reading"));
        f.close();
        return false;
    }

    QElapsedTimer elapsed;
    QProgressDialog progress;

    progress.setWindowTitle(tr("Reading file"));
    progress.setLabelText(tr("Reading data - ") + m_filename);

    progress.setMinimum(0);
    progress.setMaximum(fileSize);
    progress.setValue(0);

    elapsed.restart();

    progress.show();

    QString delimiter = getDelimiterCharacter();

    qint64 byteCount = 0;
    qint64 lineCount = 0;
    qint64 badLineCount = 0;

    // Ensure we are at the start of the file
    f.seek(0);

    QStringList row;

    while (!f.atEnd() && !progress.wasCanceled())
    {
        // Update progress bar periodically
        if (elapsed.elapsed() > 250)
        {
            progress.setValue(byteCount);

            QApplication::processEvents();

            elapsed.restart();
        }

        QByteArray bytes = f.readLine();

        QString line(bytes);

        line = line.trimmed();

        // Ignore lines which start with prohibited characters
        if (!m_options.ignoreRowsStartingWith.isEmpty() && line.startsWith(m_options.ignoreRowsStartingWith)) {
            continue;
        }

        row = line.split(delimiter);

        if (!processRow(lineCount, row, errors))
        {
            badLineCount++;
        }

        byteCount += line.length();

        lineCount++;
    }

    // Ensure file object is closed
    f.close();

    if (badLineCount > 0)
    {
        errors.append(QString("Lines with errors: " + QString::number(badLineCount)));
    }

    if (progress.wasCanceled())
    {
        errors.append(tr("File import was cancelled"));
        return false;
    }
    else
    {
        progress.close();
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
bool LumberjackCSVImporter::processRow(int rowIndex, const QStringList &row, QStringList& errors)
{

    if (rowIndex == m_options.rowHeaders)
    {
        return extractHeaders(rowIndex, row, errors);
    }
    else if (rowIndex == m_options.rowUnits)
    {
        // TODO - Extract units data

        return true;
    }
    else
    {
        return extractData(rowIndex, row, errors);
    }
}


/**
 * @brief LumberjackCSVImporter::extractHeaders - Extract headers from the provided file
 * @param rowIndex
 * @param row
 * @param errors
 * @return
 */
bool LumberjackCSVImporter::extractHeaders(int rowIndex, const QStringList &row, QStringList &errors)
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
            qDebug() << "CSV: New data series:" << header;

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
bool LumberjackCSVImporter::extractData(int rowIndex, const QStringList &row, QStringList &errors)
{
    double timestamp = 0;

    QString text;
    double value = 0;
    bool result = false;

    if (!m_options.hasTimestamp)
    {
        switch (m_options.timestampFormat)
        {
        case CSVImportOptions::TimestampFormat::SECONDS:
        default:
            timestamp = (incrementingTimestamp += 1.0f);
            break;
        case CSVImportOptions::MILLISECONDS:
            timestamp = (incrementingTimestamp += 0.001f);
            break;
        }

        timestamp = incrementingTimestamp++;
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

    double timestampScaler = getTimestampScaler();

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



/**
 * Return the list of imported data series
 */
QList<QSharedPointer<DataSeries>> LumberjackCSVImporter::getDataSeries(void) const
{
    return columnMap.values();
}
