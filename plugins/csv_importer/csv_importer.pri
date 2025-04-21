INCLUDEPATH += ./plugins/csv_importer

HEADERS += \
    ./plugins/csv_importer/lumberjack_csv_importer.hpp \
    ./plugins/csv_importer/import_options_dialog.hpp \
    ./plugins/csv_importer/csv_import_options.hpp

SOURCES += \
    ./plugins/csv_importer/lumberjack_csv_importer.cpp \
    ./plugins/csv_importer/import_options_dialog.cpp

FORMS += \
    ./plugins/csv_importer/ui/csv_import_options.ui
