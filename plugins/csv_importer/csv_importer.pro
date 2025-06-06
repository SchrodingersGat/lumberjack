QT += gui widgets

TEMPLATE = lib
DEFINES += CSV_IMPORTER_LIBRARY

CONFIG += c++17
CONFIG -= debug_and_release

# Error if non-void func does not have a return type
QMAKE_CXXFLAGS += -Wreturn-type -Werror=return-type

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    ../../src \
    ../../src/plugins

HEADERS += \
    csv_importer_global.h \
    lumberjack_csv_import_plugin.hpp \
    lumberjack_csv_importer.hpp \
    import_options_dialog.hpp \
    csv_import_options.hpp \
    ../../src/data_series.hpp \
    ../../src/plugins/plugin_base.hpp \
    ../../src/plugins/plugin_importer.hpp \

SOURCES += \
    ../../src/data_series.cpp \
    ../../src/plugins/plugin_importer.cpp \
    import_options_dialog.cpp \
    lumberjack_csv_importer.cpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}

# Specify output directory
CONFIG(debug, debug|release) {
    CONFIG += debug
    DESTDIR = build/debug

} else {
    CONFIG += release
    DESTDIR = ../build/release
}

RCC_DIR = $$DESDIR
MOC_DIR = $$DESTDIR/moc
OBJECTS_DIR = $$DESTDIR/objects

# Location of generated ui_xxxx.h files
UI_DIR = build/ui

!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lumberjack_csv_viewer.json

FORMS += \
    ui/csv_import_options.ui
