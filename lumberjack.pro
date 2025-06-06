QT       += core gui opengl svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x050F00

CONFIG += c++11
CONFIG += file_copies
CONFIG -= debug_and_release

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_CXXFLAGS += -Wreturn-type -Werror=return-type
QMAKE_LFLAGS += --verbose

INCLUDEPATH += ./qwt/src
DEPENDPATH += ./qwt/src

# Dynamic linking for qwt libraries
include(qwt/qwt.prf)

CONFIG (debug, debug|release) {
    LIBS += -L./qwt/lib -lqwtd
} else {
    LIBS += -L./qwt/lib -lqwt
}

# Includes for default plugins
include("plugins/plugins.pri")

INCLUDEPATH += src \
    src/plugins \
    src/widgets \
    fft/include/simple_fft \
    plugins \
    plugins/cedat_protocol \
    plugins/cobsr

SOURCES += \
    src/data_source_manager.cpp \
    src/fft_sampler.cpp \
    src/fft_widget.cpp \
    src/helpers.cpp \
    src/data_series.cpp \
    src/data_source.cpp \
    src/lumberjack_debug.cpp \
    src/lumberjack_settings.cpp \
    src/lumberjack_version.cpp \
    src/plot_curve.cpp \
    src/plot_legend.cpp \
    src/plot_marker.cpp \
    src/plot_widget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/plugins/plugin_exporter.cpp \
    src/plugins/plugin_importer.cpp \
    src/plugins/plugin_registry.cpp \
    src/widgets/about_dialog.cpp \
    src/widgets/axis_edit_dialog.cpp \
    src/widgets/datatable_widget.cpp \
    src/widgets/dataview_tree.cpp \
    src/widgets/dataview_widget.cpp \
    src/widgets/debug_widget.cpp \
    src/widgets/plot_sampler.cpp \
    src/widgets/plugins_dialog.cpp \
    src/widgets/series_editor_dialog.cpp \
    src/widgets/stats_widget.cpp \
    src/widgets/timeline_widget.cpp

HEADERS += \
    src/data_source_manager.hpp \
    src/mainwindow.h \
    src/fft_sampler.hpp \
    src/fft_widget.hpp \
    src/helpers.hpp \
    src/data_series.hpp \
    src/data_source.hpp \
    src/lumberjack_debug.hpp \
    src/lumberjack_settings.hpp \
    src/lumberjack_version.hpp \
    src/plot_curve.hpp \
    src/plot_legend.hpp \
    src/plot_marker.hpp \
    src/plot_panner.hpp \
    src/plot_widget.hpp \
    src/plugins/plugin_base.hpp \
    src/plugins/plugin_exporter.hpp \
    src/plugins/plugin_filter.hpp \
    src/plugins/plugin_importer.hpp \
    src/plugins/plugin_registry.hpp \
    src/widgets/about_dialog.hpp \
    src/widgets/axis_edit_dialog.hpp \
    src/widgets/datatable_widget.hpp \
    src/widgets/dataview_tree.hpp \
    src/widgets/dataview_widget.hpp \
    src/widgets/debug_widget.hpp \
    src/widgets/plot_sampler.hpp \
    src/widgets/plugins_dialog.hpp \
    src/widgets/series_editor_dialog.hpp \
    src/widgets/stats_widget.hpp \
    src/widgets/timeline_widget.hpp \

# simple-fft includes
HEADERS += \
    fft/include/simple_fft/check_fft.hpp \
    fft/include/simple_fft/copy_array.hpp \
    fft/include/simple_fft/error_handling.hpp \
    fft/include/simple_fft/fft.h \
    fft/include/simple_fft/fft.hpp \
    fft/include/simple_fft/fft_impl.hpp \
    fft/include/simple_fft/fft_settings.h

FORMS += \
    ui/about_dialog.ui \
    ui/axis_edit_dialog.ui \
    ui/curve_editor_dialog.ui \
    ui/dataview_widget.ui \
    ui/debug_widget.ui \
    ui/mainwindow.ui \
    ui/plugins_dialog.ui \
    ui/stats_view.ui

# Application icon
RC_ICONS = logo/lumberjack.ico

#Set the location for the generated ui_xxxx.h files
UI_DIR = build/ui

# Add some build info defines
DEFINES += COMPILER=\\\"$$QMAKE_CXX\\\" \
           COMPILER_VERSION=\\\"$$system($$QMAKE_CXX ' -dumpversion')\\\" \
           ARCHITECTURE=\\\"$$QT_ARCH\\\" \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Configure output directories
CONFIG(debug, debug|release) {
    CONFIG += debug
    DESTDIR = build/debug
    UI_DIR = build/tmp/debug/ui
    MOC_DIR = build/tmp/debug/moc
    OBJECTS_DIR = build/tmp/debug/objects
    RCC_DIR = build/tmp/debug/rcc
} else {
    CONFIG += release
    DESTDIR = build/release
    UI_DIR = build/tmp/release/ui
    MOC_DIR = build/tmp/release/moc
    OBJECTS_DIR = build/tmp/release/objects
    RCC_DIR = build/tmp/release/rcc
}
dllFiles.path = $$DESTDIR

COPIES += dllFiles

# Copy required .dll files across
CONFIG(debug, debug|release) {
    win32 {
        dllFiles.files += \
            qwt/lib/qwtd.dll
    } else {
        dllFiles.files += \
            qwt/lib/libqwtd.so
    }
} else {
    win32 {
        dllFiles.files += \
            qwt/lib/qwt.dll
    } else {
        dllFiles.files += \
            qwt/lib/libqwt.so
    }
}

COPIES += dllFiles

CONFIG(debug, debug | release) {
    win32 {
        # Copy required .DLL files
        QMAKE_POST_LINK += $$[QT_INSTALL_BINS]\windeployqt --opengl --openglwidgets --widgets --compiler-runtime $$shell_path($$quote($$DESTDIR))\lumberjack.exe $$escape_expand(\n\t)
    }
} else {
    win32 {
        # Copy required .DLL files
        QMAKE_POST_LINK += $$[QT_INSTALL_BINS]\windeployqt --release --opengl --openglwidgets --widgets --compiler-runtime $$shell_path($$quote($$DESTDIR))\lumberjack.exe $$escape_expand(\n\t)
    }
}

RESOURCES += \
    resources.qrc
