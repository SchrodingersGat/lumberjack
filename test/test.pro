QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable
QMAKE_LFLAGS += --verbose

# qwt toolkit
INCLUDEPATH += ../qwt-6.1.4/src
INCLUDEPATH += ../qwt-6.1.4
DEPENDPATH += ../qwt-6.1.4/src

include("qwt.pro")

INCLUDEPATH += ../src

# googletest
INCLUDEPATH += ../gtest/googletest \
               ../gtest/googletest/include
DEPENDPATH += ../gtest/googletest/src

SOURCES += \
        ../gtest/googletest/src/gtest-all.cc \
        ../gtest/googletest/src/gtest-death-test.cc \
        ../gtest/googletest/src/gtest-filepath.cc \
        ../gtest/googletest/src/gtest-matchers.cc \
        ../gtest/googletest/src/gtest-port.cc \
        ../gtest/googletest/src/gtest-printers.cc \
        ../gtest/googletest/src/gtest-test-part.cc \
        ../gtest/googletest/src/gtest-typed-test.cc \
        ../gtest/googletest/src/gtest.cc \
        ../gtest/googletest/src/gtest_main.cc \
        ../src/series.cpp \
        test_series.cpp

HEADERS += \
    ../gtest/googletest/include/gtest/gtest-death-test.h \
    ../gtest/googletest/include/gtest/gtest-matchers.h \
    ../gtest/googletest/include/gtest/gtest-message.h \
    ../gtest/googletest/include/gtest/gtest-param-test.h \
    ../gtest/googletest/include/gtest/gtest-printers.h \
    ../gtest/googletest/include/gtest/gtest-spi.h \
    ../gtest/googletest/include/gtest/gtest-test-part.h \
    ../gtest/googletest/include/gtest/gtest-typed-test.h \
    ../gtest/googletest/include/gtest/gtest.h \
    ../gtest/googletest/include/gtest/gtest_pred_impl.h \
    ../gtest/googletest/include/gtest/gtest_prod.h \
    ../gtest/googletest/src/gtest-internal-inl.h \
    ../src/series.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
