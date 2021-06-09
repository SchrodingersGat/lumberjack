#ifndef LUMBERJACK_SETTINGS_HPP
#define LUMBERJACK_SETTINGS_HPP

#include <QSettings>


class LumberjackSettings
{
    static LumberjackSettings* instance;
public:
    LumberjackSettings();

    static QString getSettingsDirectory(void);
    static QString getSettingsFile(void);

    // Singleton design pattern
    static LumberjackSettings* getInstance()
    {
        if (!instance)
        {
            instance = new LumberjackSettings;
            instance->createDirectories();
        }

        return instance;
    }

    QVariant loadSetting(QString group, QString key, QVariant defaultValue);
    void saveSetting(QString group, QString key, QVariant value);

protected:
    void createDirectories(void);

    QSettings settings;
};



#endif // LUMBERJACK_SETTINGS_HPP
