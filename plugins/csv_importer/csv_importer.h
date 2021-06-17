#ifndef CSV_IMPORTER_H
#define CSV_IMPORTER_H

#include "data_source.hpp"


class CSV_Importer : public DataSource, DataSourcePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DATASOURCE_PLUGIN_IID)
    Q_INTERFACES(DataSourcePlugin)

public:
    CSV_Importer();

    virtual QString getPluginVersion(void) override { return "0.0.1"; }
    virtual QString getPluginDescription(void) override { return "CSV Importer"; }

    virtual QStringList getSupportedFileTypes(void) override;

    virtual bool loadFromFile(const QString filename) override;
};

#endif // CSV_IMPORTER_H
