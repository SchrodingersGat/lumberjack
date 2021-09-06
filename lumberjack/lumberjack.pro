QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_LFLAGS += --verbose

# Dynamic linking for qwt libraries
include(../qwt/qwt.prf)

Release:DESTDIR = release
Debug:DESTDIR = debug

INCLUDEPATH += ../src \
               widgets \
               plugins \
               plugins/cedat_protocol \
               plugins/cobsr

SOURCES += \
    ../src/helpers.cpp \
    ../src/data_series.cpp \
    ../src/data_source.cpp \
    ../src/lumberjack_settings.cpp \
    ../src/plot_curve.cpp \
    ../src/plot_widget.cpp \
    main.cpp \
    mainwindow.cpp \
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
    widgets/about_dialog.cpp \
    widgets/axis_scale_dialog.cpp \
    widgets/datatable_widget.cpp \
    widgets/dataview_tree.cpp \
    widgets/dataview_widget.cpp \
    widgets/series_editor_dialog.cpp \
    widgets/stats_widget.cpp \
    widgets/timeline_widget.cpp

HEADERS += \
    ../src/helpers.hpp \
    ../src/data_series.hpp \
    ../src/data_source.hpp \
    ../src/lumberjack_settings.hpp \
    ../src/lumberjack_version.hpp \
    ../src/plot_curve.hpp \
    ../src/plot_widget.hpp \
    mainwindow.h \
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
    widgets/about_dialog.hpp \
    widgets/axis_scale_dialog.hpp \
    widgets/datatable_widget.hpp \
    widgets/dataview_tree.hpp \
    widgets/dataview_widget.hpp \
    widgets/series_editor_dialog.hpp \
    widgets/stats_widget.hpp \
    widgets/timeline_widget.hpp

FORMS += \
    about_dialog.ui \
    axis_scale_dialog.ui \
    curve_editor_dialog.ui \
    dataview_widget.ui \
    mainwindow.ui \
    stats_view.ui

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
