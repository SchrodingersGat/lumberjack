#ifndef CSV_EXPORTER_GLOBAL_H
#define CSV_EXPORTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CSV_EXPORTER_LIBRARY)
#define CUSTOM_FILTER_EXPORT Q_DECL_EXPORT
#else
#define CUSTOM_FILTER_EXPORT Q_DECL_IMPORT
#endif

#endif // CSV_EXPORTER_GLOBAL_H