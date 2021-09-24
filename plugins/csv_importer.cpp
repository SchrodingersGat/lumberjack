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

    qDebug() << "read" << lineCount << "lines";

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

        if (!series.isNull())
        {
            // TODO: Do something about duplicate headers!
            qWarning() << "Found duplicate header!";
        }

        addSeries(header);
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

        QString header = headers.at(ii);

        auto series = getSeriesByLabel(header);

        if (series.isNull())
        {
            qWarning() << "Could not find series matching label" << header;
            continue;
        }

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

    timestamp = ts.toDouble(&result);

    if (result)
    {
        timestamp *= timestampScaler;
    }

    return result;
}
