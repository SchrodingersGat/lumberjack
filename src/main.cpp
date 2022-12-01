#include <QApplication>
#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "lumberjack_version.hpp"

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Configure application properties
    a.setApplicationName("lumberjack");
    a.setApplicationDisplayName("lumberjack");
    a.setApplicationVersion(LUMBERJACK_VERSION_STRING);

    QCoreApplication::setApplicationName("Lumberjack");
    QCoreApplication::setApplicationVersion(LUMBERJACK_VERSION_STRING);

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Lumberjack");
    parser.addHelpOption();
    parser.addVersionOption();

    // Boolean option to add dummy data
    QCommandLineOption dummyDataOption(QStringList() << "d" << "dummy", "Load dummy test data");

    parser.addOption(dummyDataOption);

    parser.process(a);

    MainWindow w;
    w.show();

    if (parser.isSet(dummyDataOption))
    {
        w.loadDummyData();
    }

    return a.exec();
}
