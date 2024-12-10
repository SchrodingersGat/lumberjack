#include <QFileInfo>

#include "plugin_importer.hpp"

/**
 * @brief PluginImporter::supportsFileType - Determine if a particular file type is supported
 * @param fileType - The file extension e.g. 'csv'
 * @return
 */
bool ImportPlugin::supportsFileType(QString fileType) const
{
    fileType = fileType.replace(".", "").toLower();

    for (QString sft : supportedFileTypes())
    {
        if (sft.replace(".", "").toLower() == fileType)
        {
            return true;
        }
    }

    return false;
}


/**
 * @brief ImportPlugin::fileFilter constructs a file filter string for importing files
 * @return
 */
QString ImportPlugin::fileFilter(void) const
{
    QString filter = pluginName();

    filter += " (";

    QStringList fileFilters;

    for (QString ft : supportedFileTypes())
    {
        if (!ft.startsWith(".")) ft.prepend(".");
        ft.prepend("*");

        fileFilters.append(ft);
    }

    filter += fileFilters.join(" ");
    filter += ")";

    return filter;
}


/**
 * @brief ImportPlugin::validateFile - Check that a file can be opened and is otherwise valid
 * @param filename
 * @return
 */
bool ImportPlugin::validateFile(QString filename, QStringList &errors) const
{
    QFileInfo info(filename);

    if (!info.exists())
    {
        errors.append(tr("File does not exist"));
    }

    if (!info.isFile())
    {
        errors.append(tr("Not a valid file"));
    }

    if (!info.isReadable())
    {
        errors.append(tr("File is not readable"));
    }

    if (info.size() == 0) {
        errors.append(tr("File is empty"));
    }

    return errors.count() == 0;
}
