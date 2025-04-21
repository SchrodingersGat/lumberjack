#ifndef LUMBERJACK_CSV_IMPORT_PLUGIN_HPP
#define LUMBERJACK_CSV_IMPORT_PLUGIN_HPP

#include "lumberjack_csv_importer.hpp"
#include "csv_importer_global.h"


/**
 * Plugin interface definition for the LumberjackCSVImporter
 * Use this to compile as a standalone plugin
 */
class CSV_IMPORTER_EXPORT LumberjackCSVImporterPlugin : public LumberjackCSVImporter
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ImporterInterface_iid)
    Q_INTERFACES(ImportPlugin)
};

#endif // LUMBERJACK_CSV_IMPORT_PLUGIN_HPP
