#ifndef LUMBERJACK_CSV_IMPORT_PLUGIN_HPP
#define LUMBERJACK_CSV_IMPORT_PLUGIN_HPP

#include "lumberjack_csv_exporter.hpp"
#include "csv_exporter_global.h"


/**
 * Plugin interface definition for the LumberjackCSVExporter
 * Use this to compile as a standalone plugin
 */
class CSV_EXPORTER_EXPORT LumberjackCSVExporterPlugin : public LumberjackCSVExporter
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ExporterInterface_iid)
    Q_INTERFACES(ExportPlugin)
};

#endif // LUMBERJACK_CSV_IMPORT_PLUGIN_HPP
