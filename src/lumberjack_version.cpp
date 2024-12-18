#include <QSettings>


#include "lumberjack_version.hpp"

static QString _version;


QString getLumberjackVersion(void)
{
    if (_version.isEmpty())
    {
        QSettings settings(":/resource/version.ini", QSettings::IniFormat);
        _version = settings.value("version", "_").toString();
    }

    if (_version == "_") {
        return QString();
    }

    return _version;
}
