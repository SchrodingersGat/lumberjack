QT += widgets

TEMPLATE = lib
DEFINES += CSV_IMPORTER_LIBRARY

CONFIG += c++11
CONFIG += plugin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../../src

SOURCES += \
    ../../src/data_series.cpp \
    ../../src/data_source.cpp \
    csv_importer.cpp

HEADERS += \
    ../../src/data_series.hpp \
    ../../src/data_source.hpp \
    csv_importer.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
