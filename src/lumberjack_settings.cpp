#include "lumberjack_settings.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QDebug>


LumberjackSettings* LumberjackSettings::instance = 0;


LumberjackSettings::LumberjackSettings() : settings(getSettingsFile(), QSettings::IniFormat)
{
}


QString LumberjackSettings::getSettingsDirectory()
{
    auto dirs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

    if (dirs.count() > 0)
    {
        return dirs.at(0);
    }

    return QString();
}


QString LumberjackSettings::getSettingsSubdirectory(QString subdir)
{
    return LumberjackSettings::getSettingsDirectory() + QDir::separator() + subdir;
}


QString LumberjackSettings::getPluginsDirectory()
{
    return LumberjackSettings::getSettingsSubdirectory("plugins");
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
    dirs.append(getPluginsDirectory());

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


QString LumberjackSettings::loadString(QString group, QString key, QString defaultValue)
{
    return loadSetting(group, key, defaultValue).toString();
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
