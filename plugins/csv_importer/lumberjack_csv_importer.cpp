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
bool LumberjackCSVImporter::beforeLoadData(void)
{
    CSVImportOptionsDialog dlg(m_filename);

    qDebug() << "beforeLoadData";

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
    qDebug() << "loadDataFile...";

    // TODO:
    return true;
}


/**
 * Return the list of imported data series
 */
QList<QSharedPointer<DataSeries>> LumberjackCSVImporter::getDataSeries(void) const
{
    // TODO

    return QList<QSharedPointer<DataSeries>>();
}
