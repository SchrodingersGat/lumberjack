#ifndef IMPORT_OPTIONS_DIALOG_HPP
#define IMPORT_OPTIONS_DIALOG_HPP

#include <qdialog.h>

#include "csv_import_options.hpp"

#include "ui_csv_import_options.h"


class CSVImportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    CSVImportOptionsDialog(QString filename, QWidget *parent = nullptr);
    ~CSVImportOptionsDialog();

    CSVImportOptions getOptions(void) { return m_options; }

public slots:
    void saveImportOptions(void);
    void updateImportPreview(void);

protected:

    QStringList getDataHeaders(void);

    void readFileHeader(void);

    void initImportOptions(void);


    //! The filename to load data from
    QString m_filename;

    //! The first n lines of the file
    QStringList m_fileHeader;

    CSVImportOptions m_options;

    //! Dialog UI
    Ui::csvImportOptionsDialog ui;
};


#endif // IMPORT_OPTIONS_DIALOG_HPP
