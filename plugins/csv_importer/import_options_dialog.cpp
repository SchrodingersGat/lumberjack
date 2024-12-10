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

    initImportPreview();
    readFileHeader();
    updateImportPreview();

    connect(ui.cancelButton, &QPushButton::released, this, &CSVImportOptionsDialog::reject);
    connect(ui.importButton, &QPushButton::released, this, &CSVImportOptionsDialog::accept);
}


CSVImportOptionsDialog::~CSVImportOptionsDialog()
{
    // TODO
}


/**
 * @brief CSVImportOptionsDialog::getOptions - Return the selected options
 * @return
 */
CSVImportOptions CSVImportOptionsDialog::getOptions() const
{
    CSVImportOptions options;

    options.delimeter = (CSVImportOptions::DelimiterType) ui.columnDelimiter->currentIndex();
    options.timestampFormat = (CSVImportOptions::TimestampFormat) ui.timestampFormat->currentIndex();

    // TODO: The rest of the options...

    return options;
}


void CSVImportOptionsDialog::initImportOptions()
{
    ui.columnDelimiter->addItem(tr("Comma"));
    ui.columnDelimiter->addItem(tr("Tab"));
    ui.columnDelimiter->addItem(tr("Colon"));
    ui.columnDelimiter->addItem(tr("Pipe"));
    ui.columnDelimiter->addItem(tr("Space"));

    ui.timestampFormat->addItem(tr("Seconds"));
    ui.timestampFormat->addItem(tr("Milliseconds"));
    ui.timestampFormat->addItem(tr("hh:mm:ss"));
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


void CSVImportOptionsDialog::initImportPreview(void)
{
    QTableWidget *table = ui.previewTable;

    if (!table) return;
}


void CSVImportOptionsDialog::updateImportPreview(void)
{
    QTableWidget *table = ui.previewTable;

    if (!table) return;
}
