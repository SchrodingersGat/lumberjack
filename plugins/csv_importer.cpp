#include <math.h>

#include <qfile.h>
#include <qdebug.h>
#include <qelapsedtimer.h>
#include <qprogressdialog.h>
#include <qapplication.h>

#include "csv_importer.hpp"


CSVImportOptions::CSVImportOptions()
{
}


CSVImportOptions::~CSVImportOptions()
{

}


/*
 * Return the delimiter string based on the selected option
 */
QString CSVImportOptions::getDelimiter() const
{
    switch (delimiter) {
    case DelimiterType::COMMA:
    default:
        return ",";
    case DelimiterType::TAB:
        return "\t";
    case DelimiterType::COLON:
        return ":";
    case DelimiterType::SEMICOLON:
        return ";";
    case DelimiterType::PIPE:
        return "|";
    case DelimiterType::SPACE:
        return " ";
    }
}


/*
 * Return the timestamp scaler value based on the selected option
 */
double CSVImportOptions::getTimestampScaler() const
{
    switch (timestampFormat) {
    case TimestampFormat::MILLISECONDS:
        return 0.001;
    case TimestampFormat::SECONDS:
    case TimestampFormat::HHMMSS:
    default:
        return 1.0;
    }
}


CSVImportOptionsDialog::CSVImportOptionsDialog(QString filename, QStringList head, QWidget *parent) :
    QDialog(parent),
    filename(filename),
    fileHead(head)
{
    ui.setupUi(this);
    setWindowModality(Qt::ApplicationModal);

    setWindowTitle(tr("CSV Data Import Options"));

    /* Initialize import options */

    // Column delimiter
    ui.columnDelimiter->addItem(tr("Comma") + " - ','");
    ui.columnDelimiter->addItem(tr("Tab") + " - '\\t'");
    ui.columnDelimiter->addItem(tr("Colon") + " - ':'");
    ui.columnDelimiter->addItem(tr("Semicolon") + " - ';'");
    ui.columnDelimiter->addItem(tr("Pipe") + " - '|'");
    ui.columnDelimiter->addItem(tr("Space") + " - ''");

    // Timestamp format
    ui.timestampFormat->addItem(tr("Seconds"), CSVImportOptions::TimestampFormat::SECONDS);
    ui.timestampFormat->addItem(tr("Milliseconds"), CSVImportOptions::TimestampFormat::MILLISECONDS);
    ui.timestampFormat->addItem(tr("hh:mm::ss"), CSVImportOptions::TimestampFormat::HHMMSS);

    // Connect signals / slots
    connect(ui.cancelButton, &QPushButton::released, this, &QDialog::reject);
    connect(ui.importButton, &QPushButton::released, this, &CSVImportOptionsDialog::importData);

    for (QString line : fileHead) {
        ui.filePreview->append(line.trimmed());
    }

}

CSVImportOptionsDialog::~CSVImportOptionsDialog()
{
    // TODO
}



void CSVImportOptionsDialog::importData()
{
    // Copy across settings!
    options.zeroInitialTimestamp = ui.zeroInitialTimestamp->isChecked();
    options.headerRow = ui.dataLabelRow->value();
    options.unitsRow = ui.dataUnitsRow->value();
    options.timestampFormat = ui.timestampFormat->currentIndex();
    options.delimiter = ui.columnDelimiter->currentIndex();
    options.ignoreRowsStartingWith = ui.ignoreStartWith->text().trimmed();

    // Allow for data files which do not have a timestamp column
    if (ui.missingTimestampColumn->isChecked()) {
        options.timestampColumn = -1;
    } else {
        options.timestampColumn = ui.timestampColumn->value();
    }

    accept();
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


/*
 * Select import options.
 * Opens a dialog box to specify options for importing data
 */
bool CSVImporter::setImportOptions()
{

    QStringList head = getFileHead(filename);

    if (head.count() == 0) {
        qCritical() << "Could not extract header from" << filename;
        return false;
    }

    auto *dlg = new CSVImportOptionsDialog(filename, head);

    dlg->setAttribute(Qt::WA_DeleteOnClose, false);

    bool result = dlg->exec() == QDialog::Accepted;

    if (result) {
        // Copy across import options
        importOptions = dlg->options;
    }

    dlg->deleteLater();
    return result;

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

        // Ignore lines which start with prohibited characters
        if (!importOptions.ignoreRowsStartingWith.isEmpty() && line.startsWith(importOptions.ignoreRowsStartingWith)) {
            continue;
        }

        QString delimiter = importOptions.getDelimiter();

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
    if (rowIndex == importOptions.headerRow)
    {
        return extractHeaders(rowIndex, row, errors);
    }
    else if (rowIndex == importOptions.unitsRow)
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
        if (ii == importOptions.timestampColumn)
        {
            continue;
        }

        // Check that we don't have a duplicate header already
        auto series = getSeriesByLabel(header);

        while (!getSeriesByLabel(header).isNull())
        {
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

    if (importOptions.timestampColumn == -1) {
        timestamp = incrementingTimestamp++;
    } else if (!extractTimestamp(rowIndex, row, timestamp))
    {
        qWarning() << "Line" << rowIndex << "does not contain valid timestamp";
        return false;
    }

    if (!initialTimestampSeen) {
        initialTimestampSeen = true;
        initialTimetamp = timestamp;
    }

    if (importOptions.timestampColumn >= 0 && importOptions.zeroInitialTimestamp) {
        timestamp -= initialTimetamp;
    }

    for (int ii = 0; ii < row.length(); ii++)
    {
        // Ignore the timestamp column
        if (ii == importOptions.timestampColumn)
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
    if (row.length() <= importOptions.timestampColumn)
    {
        qWarning() << "Line" << rowIndex << "missing timestamp column";
        return false;
    }

    QString ts = row.at(importOptions.timestampColumn);

    bool result = false;

    double timestampScaler = importOptions.getTimestampScaler();

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
