QT += core gui testlib

CONFIG += c++11 console
CONFIG += testcase

CONFIG -= app_bundle

# Code coverage support
CONFIG += gcov

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable -fprofile-arcs -ftest-coverage -lgcov
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_LFLAGS += --verbose

DEFINES += CI_UNIT_TEST

# Dynamic linking for qwt libraries
include(../qwt/qwt.prf)

CONFIG (debug, debug|release) {
    LIBS += -L../qwt/lib -lqwtd
} else {
    LIBS += -L../qwt/lib -lqwt
}

INCLUDEPATH += ../qwt/src

INCLUDEPATH += ../src

SOURCES += \
    ../src/data_series.cpp \
    ../src/data_source.cpp \
    ../src/plot_curve.cpp \
    main.cpp \

HEADERS += \
    ../src/data_series.hpp \
    ../src/data_source.hpp \
    ../src/lumberjack_version.hpp \
    ../src/plot_curve.hpp \
    test_curve.hpp \
    test_series.hpp \
    test_source.hpp

# Generate coverage data
QMAKE_CXXFLAGS += --coverage
QMAKE_LFLAGS += --coverage

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
