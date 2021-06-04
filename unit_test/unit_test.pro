QT += core gui testlib

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_LFLAGS += --verbose

# qwt toolkit
INCLUDEPATH += ../qwt-6.1.4/src
INCLUDEPATH += ../qwt-6.1.4
DEPENDPATH += ../qwt-6.1.4/src

INCLUDEPATH += ../

include("qwt.pro")

INCLUDEPATH += ../src

SOURCES += \
    ../src/data_series.cpp \
    ../src/data_source.cpp \
    main.cpp \

HEADERS += \
    ../src/data_series.hpp \
    ../src/data_source.hpp \
    test_series.hpp \
    test_source.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
