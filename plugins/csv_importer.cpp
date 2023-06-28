#include <math.h>

#include <qfile.h>
#include <qelapsedtimer.h>
#include <qprogressdialog.h>
#include <qapplication.h>

#include "csv_importer.hpp"


CSVImportOptionsDialog::CSVImportOptionsDialog(QString filename, QWidget *parent) : QDialog(parent), filename(filename)
{
    ui.setupUi(this);
    setWindowModality(Qt::ApplicationModal);

    setWindowTitle(tr("CSV Data Import Options"));

    /* Initialize import options */

    // Data label row selection
    ui.dataLabelRow->setMinimum(0);
    ui.dataLabelRow->setMaximum(100);
    ui.dataLabelRow->setValue(1);

    // Data units row selection
    ui.dataUnitsRow->setMinimum(0);
    ui.dataUnitsRow->setMaximum(100);
    ui.dataUnitsRow->setValue(0);

    // Data start row selection
    ui.dataStartRow->setMinimum(1);
    ui.dataStartRow->setMaximum(100);
    ui.dataStartRow->setValue(2);

    // Ignore rows starting with
    ui.ignoreStartWith->clear();

    // Column delimiter
    ui.columnDelimiter->addItem(tr("Comma") + " - ','");
    ui.columnDelimiter->addItem(tr("Tab") + " - '\\t'");
    ui.columnDelimiter->addItem(tr("Colon") + " - ':'");
    ui.columnDelimiter->addItem(tr("Semicolon") + " - ';'");
    ui.columnDelimiter->addItem(tr("Pipe") + " - '|'");
    ui.columnDelimiter->addItem(tr("Space") + " - ''");
    ui.columnDelimiter->addItem(tr("Whitespace"));

    // Timestamp format
    ui.timestampFormat->addItem(tr("Seconds"), CSVImporter::SECONDS);
    ui.timestampFormat->addItem(tr("Milliseconds"), CSVImporter::MILLISECONDS);
    ui.timestampFormat->addItem(tr("hh:mm::ss"), CSVImporter::HHMMSS);
}

CSVImportOptionsDialog::~CSVImportOptionsDialog()
{
    // TODO
}



CSVImporter::CSVImporter() : FileDataSource("CSV Importer")
{

}


CSVImporter::~CSVImporter()
{
    // TODO - Any custom cleanup?
}


QStringList CSVImporter::getSupportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes.append("csv");
    fileTypes.append("tsv");
    fileTypes.append("tab");

    return fileTypes;
}


bool CSVImporter::setImportOptions()
{
    return true;

    // TODO - Set CSV import options

    auto *dlg = new CSVImportOptionsDialog(filename);

    int result = dlg->exec();

    dlg->deleteLater();

    return result == QDialog::Accepted;
}


bool CSVImporter::loadDataFromFile(QStringList &errors)
{
    QFile f(filename);

    qint64 fileSize = getFileSize();

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
    progress.setLabelText(tr("Reading data - ") + filename);

    progress.setMinimum(0);
    progress.setMaximum(fileSize);
    progress.setValue(0);

    elapsed.restart();

    progress.show();

    qint64 byteCount = 0;
    qint64 lineCount = 0;

    // Ensure we are at the start of the file
    f.seek(0);

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

        QStringList row = line.split(delimiter);

        if (!processRow(lineCount, row, errors))
        {
            // TODO: What happens when a row goes bad?
        }

        byteCount += line.length();

        lineCount++;
    }

    // Ensure file object is closed
    f.close();

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


bool CSVImporter::processRow(int rowIndex, const QStringList &row, QStringList& errors)
{
    if (rowIndex == headerRow)
    {
        return extractHeaders(rowIndex, row, errors);
    }
    else if (rowIndex == unitsRow)
    {
        // TODO - Extract units data

        return true;
    }
    else
    {
        return extractData(rowIndex, row, errors);
    }
}


bool CSVImporter::extractHeaders(int rowIndex, const QStringList &row, QStringList &errors)
{
    Q_UNUSED(rowIndex);

    qDebug() << "extractHeaders";

    headers.clear();

    for (int ii = 0; ii < row.length(); ii++)
    {
        QString header = row.at(ii).trimmed();

        if (header.isEmpty())
        {
            qWarning() << "Empty header in column" << ii;

            // Default column headers
            header = "Column " + QString::number(ii);
        }

        headers.append(header);

        // Ignore timestamp column
        if (ii == timestampColumn)
        {
            continue;
        }

        // Check that we don't have a duplicate header already
        auto series = getSeriesByLabel(header);

        while (!getSeriesByLabel(header).isNull())
        {
            // TODO: Do something about duplicate headers!
            qWarning() << "Found duplicate header:" << header;
            header += "_2";
        }

        addSeries(header);

        columnMap[header] = getSeriesByLabel(header);
    }

    return true;
}


bool CSVImporter::extractData(int rowIndex, const QStringList &row, QStringList &errors)
{

    double timestamp = 0;

    QString text;
    double value = 0;
    bool result = false;

    if (!extractTimestamp(rowIndex, row, timestamp))
    {
        qWarning() << "Line" << rowIndex << "does not contain valid timestamp";
        return false;
    }

    for (int ii = 0; ii < row.length(); ii++)
    {
        // Ignore the timestamp column
        if (ii == timestampColumn)
        {
            continue;
        }

        QString text = row.at(ii).trimmed();

        if (ii >= headers.length())
        {
            qWarning() << "Line" << rowIndex << "exceeded header count";
            continue;
        }

        // Ignore empty cell values
        if (text.isEmpty())
        {
            continue;
        }

        value = text.toDouble(&result);

        // Data could not be converted to a number
        if (!result) continue;

        // Ignore invalid or infinite values
        if (isnan(value) || isinf(value)) continue;

        QString header = headers.at(ii);

        if (!columnMap.contains(header))
        {
            qWarning() << "Could not find series matching label" << header;
            continue;
        }

        auto series = columnMap[header];

        series->addData(timestamp, value, false);
    }

    return true;
}


bool CSVImporter::extractTimestamp(int rowIndex, const QStringList &row, double &timestamp)
{
    if (row.length() <= timestampColumn)
    {
        qWarning() << "Line" << rowIndex << "missing timestamp column";
        return false;
    }

    QString ts = row.at(timestampColumn);

    bool result = false;

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

            qDebug() << ts << "->" << timestamp;

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
