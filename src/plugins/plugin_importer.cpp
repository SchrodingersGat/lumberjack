#include "plugin_importer.hpp"

/**
 * @brief PluginImporter::supportsFileType - Determine if a particular file type is supported
 * @param fileType - The file extension e.g. 'csv'
 * @return
 */
bool ImporterPlugin::supportsFileType(QString fileType) const
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
