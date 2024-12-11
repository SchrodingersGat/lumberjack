#include "plugin_exporter.hpp"


bool ExportPlugin::supportsFileType(QString fileType) const
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


QString ExportPlugin::fileFilter(void) const
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
