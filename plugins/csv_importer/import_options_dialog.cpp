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


}


CSVImportOptionsDialog::~CSVImportOptionsDialog()
{
    // TODO
}


void CSVImportOptionsDialog::initImportOptions()
{
    // TODO
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
