#include <QFile>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "import_options_dialog.hpp"

CSVImportOptionsDialog::CSVImportOptionsDialog(QString filename, QWidget *parent) :
    QDialog(parent),
    m_filename(filename)
{
    ui.setupUi(this);

    // TODO: Load last selected CSV options from settings
    initImportOptions();

    readFileHeader();

    updateImportPreview();

    connect(ui.cancelButton, &QPushButton::released, this, &CSVImportOptionsDialog::reject);
    connect(ui.importButton, &QPushButton::released, this, &CSVImportOptionsDialog::saveImportOptions);

    // Callbacks to update the data preview
    connect(ui.columnDelimiter, &QComboBox::currentIndexChanged, this, &CSVImportOptionsDialog::updateImportPreview);
    connect(ui.hasTimestampColumn, &QCheckBox::released, this, &CSVImportOptionsDialog::updateImportPreview);
    connect(ui.hasHeadersRow, &QCheckBox::released, this, &CSVImportOptionsDialog::updateImportPreview);
    connect(ui.hasUnitsRow, &QCheckBox::released, this, &CSVImportOptionsDialog::updateImportPreview);
    connect(ui.timestampColumn, &QSpinBox::valueChanged, this, &CSVImportOptionsDialog::updateImportPreview);
    connect(ui.dataHeadersRow, &QSpinBox::valueChanged, this, &CSVImportOptionsDialog::updateImportPreview);
    connect(ui.dataUnitsRow, &QSpinBox::valueChanged, this, &CSVImportOptionsDialog::updateImportPreview);
}


CSVImportOptionsDialog::~CSVImportOptionsDialog()
{
    // TODO
}


void CSVImportOptionsDialog::initImportOptions()
{
    ui.columnDelimiter->addItem(tr("Comma"));
    ui.columnDelimiter->addItem(tr("Tab"));
    ui.columnDelimiter->addItem(tr("Colon"));
    ui.columnDelimiter->addItem(tr("Pipe"));
    ui.columnDelimiter->addItem(tr("Space"));

    ui.columnDelimiter->setCurrentIndex((int) m_options.delimeter);

    ui.timestampFormat->addItem(tr("Seconds"));
    ui.timestampFormat->addItem(tr("Milliseconds"));
    ui.timestampFormat->addItem(tr("hh:mm:ss"));

    ui.timestampFormat->setCurrentIndex((int) m_options.timestampFormat);

    ui.hasHeadersRow->setChecked(m_options.hasHeaders);
    ui.hasUnitsRow->setChecked(m_options.hasUnits);
    ui.hasTimestampColumn->setChecked(m_options.hasTimestamp);
    ui.zeroInitialTimestamp->setChecked(m_options.zeroTimestamp);

    ui.timestampColumn->setValue(m_options.colTimestamp);
    ui.dataHeadersRow->setValue(m_options.rowHeaders);
    ui.dataUnitsRow->setValue(m_options.rowUnits);
    ui.dataStartRow->setValue(m_options.rowDataStart);

    ui.ignoreStartWith->setText(m_options.ignoreRowsStartingWith);
}



void CSVImportOptionsDialog::saveImportOptions()
{
    CSVImportOptions options;

    options.delimeter = (CSVImportOptions::DelimiterType) ui.columnDelimiter->currentIndex();
    options.timestampFormat = (CSVImportOptions::TimestampFormat) ui.timestampFormat->currentIndex();

    options.hasHeaders = ui.hasHeadersRow->isChecked();
    options.hasUnits = ui.hasUnitsRow->isChecked();
    options.hasTimestamp = ui.hasTimestampColumn->isChecked();
    options.zeroTimestamp = ui.zeroInitialTimestamp->isChecked();

    options.colTimestamp = ui.timestampColumn->value();
    options.rowHeaders = ui.dataHeadersRow->value();
    options.rowUnits = ui.dataUnitsRow->value();
    options.rowDataStart = ui.dataStartRow->value();

    options.ignoreRowsStartingWith = ui.ignoreStartWith->text().trimmed();

    accept();
}




void CSVImportOptionsDialog::readFileHeader(void)
{
    const int N_HEADER_LINES = 20;

    QFile f(m_filename);

    m_fileHeader.clear();

    if (f.open(QIODevice::ReadOnly) && f.isOpen() && f.isReadable())
    {
        int idx = 0;

        while ((idx < N_HEADER_LINES) && !f.atEnd())
        {
            idx++;

            m_fileHeader.append(QString(f.readLine()).trimmed());
        }
    }

    f.close();
}


void CSVImportOptionsDialog::updateImportPreview(void)
{
    QTableWidget *table = ui.previewTable;

    if (!table) return;

    table->clear();

    int nRows = m_fileHeader.count();
    int nCols = 0;

    QList<QStringList> rows;

    m_options.delimeter = (CSVImportOptions::DelimiterType) ui.columnDelimiter->currentIndex();

    QString delimiter = m_options.getDelimiterString();

    for (QString row : m_fileHeader)
    {
        QStringList rowData = row.split(delimiter);

        if (rowData.count() > nCols)
        {
            nCols = rowData.count();
        }

        rows.append(rowData);
    }

    table->setRowCount(nRows);
    table->setColumnCount(nCols);

    for (int ii = 0; ii < nRows; ii++)
    {
        QStringList row = rows.at(ii);

        for (int jj = 0; jj < nCols; jj++)
        {
            QString data;

            if (jj < row.count())
            {
                data = row.at(jj);
            }

            auto *item = new QTableWidgetItem(data);

            if (ui.hasHeadersRow->isChecked() && ii == ui.dataHeadersRow->value())
            {
                item->setBackground(QColor(50, 100, 50));
            }
            else if (ui.hasUnitsRow->isChecked() && ii == ui.dataUnitsRow->value())
            {
                item->setBackground(QColor(50, 100, 100));
            }
            else if (ui.hasTimestampColumn->isChecked() && jj == ui.timestampColumn->value())
            {
                item->setBackground(QColor(100, 50, 50));
            }

            table->setItem(ii, jj, item);
        }
    }
}
