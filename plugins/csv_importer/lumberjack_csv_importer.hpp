#ifndef LUMBERJACK_CSV_IMPORTER_HPP
#define LUMBERJACK_CSV_IMPORTER_HPP

#include "csv_importer_global.h"
#include "plugin_importer.hpp"


class CSV_IMPORTER_EXPORT LumberjackCSVImporter : public Lumberjack::ImporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ImporterInterface_iid)
    Q_INTERFACES(Lumberjack::ImporterInterface)
public:
    LumberjackCSVImporter();

    // Base plugin functionality
    virtual QString pluginName(void) const override { return m_name; }
    virtual QString pluginDescription(void) const override { return m_description; }
    virtual QString pluginVersion(void) const override { return m_version; }

    // Importer plugin functionality
    virtual QStringList supportedFileTypes(void) const override;
    virtual bool loadDataFile(const QString &filename, QStringList &errors) override;

protected:
    const QString m_name = "CSV Importer";
    const QString m_description = "Import data from CSV files";
    const QString m_version = "0.1.0";

};

#endif // LUMBERJACK_CSV_IMPORTER_HPP
