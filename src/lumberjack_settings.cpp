#include "lumberjack_settings.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QDebug>


LumberjackSettings *LumberjackSettings::instance = 0;


LumberjackSettings::LumberjackSettings() : settings(getSettingsFile(), QSettings::IniFormat)
{
//    loadSettings();
}


QString LumberjackSettings::getSettingsDirectory()
{
    auto dirs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);

    if (dirs.count() > 0)
    {
        return dirs.at(0);
    }

    return QString();
}


QString LumberjackSettings::getSettingsFile()
{
    return getSettingsDirectory() + QDir::separator() + "settings.ini";
}


/*
 * Check for existing directories,
 * and create them if they do not already exist.
 */
void LumberjackSettings::createDirectories()
{
    QStringList dirs;

    dirs.append(getSettingsDirectory());

    for (auto dir : dirs)
    {
        QDir d(dir);

        if (!d.exists())
        {
            qInfo() << "Creating directory" << dir;
            d.mkpath(dir);
        }
    }
}


QVariant LumberjackSettings::loadSetting(QString group, QString key, QVariant defaultValue)
{
    QVariant result;

    settings.beginGroup(group);
    result = settings.value(key, defaultValue);
    settings.endGroup();

    return result;
}


bool LumberjackSettings::loadBoolean(QString group, QString key, bool defaultValue)
{
    QVariant result = loadSetting(group, key, defaultValue);

    return result.toBool();
}


void LumberjackSettings::saveSetting(QString group, QString key, QVariant value)
{
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
}
