QT       += core gui opengl svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x050F00

CONFIG += c++11
CONFIG += file_copies
CONFIG -= debug_and_release

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_LFLAGS += --verbose

# Dynamic linking for qwt libraries
include(qwt/qwt.prf)

CONFIG (debug, debug|release) {
    LIBS += -L./qwt/lib -lqwtd
} else {
    LIBS += -L./qwt/lib -lqwt
}

INCLUDEPATH += ./qwt/src

INCLUDEPATH += src \
               src/widgets \
               fft/include/simple_fft \
               plugins \
               plugins/cedat_protocol \
               plugins/cobsr

SOURCES += \
    plugins/mavlink_importer.cpp \
    src/fft_sampler.cpp \
    src/fft_widget.cpp \
    src/helpers.cpp \
    src/data_series.cpp \
    src/data_source.cpp \
    src/lumberjack_debug.cpp \
    src/lumberjack_settings.cpp \
    src/plot_curve.cpp \
    src/plot_legend.cpp \
    src/plot_widget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    plugins/cedat_importer.cpp \
    plugins/cedat_protocol/CEDATProtocol.cpp \
    plugins/cedat_protocol/CEDATPackets.cpp \
    plugins/cedat_protocol/fielddecode.cpp \
    plugins/cedat_protocol/fieldencode.cpp \
    plugins/cedat_protocol/scaleddecode.cpp \
    plugins/cedat_protocol/scaledencode.cpp \
    plugins/cobsr/cobs.c \
    plugins/cobsr/cobsr.c \
    plugins/csv_importer.cpp \
    src/widgets/about_dialog.cpp \
    src/widgets/axis_edit_dialog.cpp \
    src/widgets/datatable_widget.cpp \
    src/widgets/dataview_tree.cpp \
    src/widgets/dataview_widget.cpp \
    src/widgets/debug_widget.cpp \
    src/widgets/plot_sampler.cpp \
    src/widgets/series_editor_dialog.cpp \
    src/widgets/stats_widget.cpp \
    src/widgets/timeline_widget.cpp

HEADERS += \
    plugins/mavlink_importer.hpp \
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
    src/plot_panner.hpp \
    src/plot_widget.hpp \
    src/mainwindow.h \
    plugins/cedat_importer.hpp \
    plugins/cedat_protocol/CEDATProtocol.hpp \
    plugins/cedat_protocol/CEDATPackets.hpp \
    plugins/cedat_protocol/fielddecode.hpp \
    plugins/cedat_protocol/fieldencode.hpp \
    plugins/cedat_protocol/scaleddecode.hpp \
    plugins/cedat_protocol/scaledencode.hpp \
    plugins/cobsr/cobs.h \
    plugins/cobsr/cobsr.h \
    plugins/csv_importer.hpp \
    src/widgets/about_dialog.hpp \
    src/widgets/axis_edit_dialog.hpp \
    src/widgets/datatable_widget.hpp \
    src/widgets/dataview_tree.hpp \
    src/widgets/dataview_widget.hpp \
    src/widgets/debug_widget.hpp \
    src/widgets/plot_sampler.hpp \
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
    ui/csv_import_options.ui \
    ui/curve_editor_dialog.ui \
    ui/dataview_widget.ui \
    ui/debug_widget.ui \
    ui/mainwindow.ui \
    ui/stats_view.ui

# Application icon
RC_ICONS = logo/lumberjack.ico

#Set the location for the generated ui_xxxx.h files
UI_DIR = ui_tmp/

# Add some build info defines
DEFINES += COMPILER=\\\"$$QMAKE_CXX\\\" \
           COMPILER_VERSION=\\\"$$system($$QMAKE_CXX ' -dumpversion')\\\" \
           ARCHITECTURE=\\\"$$QT_ARCH\\\" \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Specify output directory
CONFIG(debug, debug|release) {
    CONFIG += debug
    DESTDIR = build/debug
    RCC_DIR = build/debug
    OBJECTS_DIR = build/debug
    MOC_DIR = build/debug/moc
} else {
    CONFIG += release
    DESTDIR = build/release
    RCC_DIR = build/release
    OBJECTS_DIR = build/release
    MOC_DIR = build/release/moc
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
            qwt/lib/libqwtd.a
    }
} else {
    win32 {
        dllFiles.files += \
            qwt/lib/qwt.dll
    } else {
        dllFiles.files += \
            qwt/lib/libqwt.a
    }
}

COPIES += dllFiles

CONFIG(debug, debug | release) {
    win32 {
        # Copy required .DLL files
        QMAKE_POST_LINK += $$[QT_INSTALL_BINS]\windeployqt --debug --compiler-runtime $$shell_path($$quote($$DESTDIR))\lumberjack.exe $$escape_expand(\n\t)
    }
} else {
    win32 {
        # Copy required .DLL files
        QMAKE_POST_LINK += $$[QT_INSTALL_BINS]\windeployqt --release --compiler-runtime $$shell_path($$quote($$DESTDIR))\lumberjack.exe $$escape_expand(\n\t)
    }
}

RESOURCES += \
    resources.qrc
