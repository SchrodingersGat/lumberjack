#include <QDir>
#include <QApplication>
#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "lumberjack_debug.hpp"
#include "lumberjack_version.hpp"
#include "lumberjack_settings.hpp"

#include "mainwindow.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // Add custom plugin dirs
    QStringList pluginPaths = a.libraryPaths();

    pluginPaths.prepend(QDir::currentPath() + "/plugins");
    pluginPaths.prepend(LumberjackSettings::getPluginsDirectory());

    a.setLibraryPaths(pluginPaths);

    QString p = QDir::currentPath() + "/plugins";

    // Configure application properties
    a.setApplicationName("lumberjack");
    a.setApplicationDisplayName("lumberjack");
    a.setApplicationVersion(getLumberjackVersion());

    QCoreApplication::setApplicationName("Lumberjack");
    QCoreApplication::setApplicationVersion(getLumberjackVersion());

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Lumberjack");
    parser.addHelpOption();
    parser.addVersionOption();

    // Command line options
    QCommandLineOption loadFilesOption(QStringList() << "f" << "file", "Load data file", "file");
    QCommandLineOption dummyDataOption(QStringList() << "d" << "dummy", "Load dummy test data");
    QCommandLineOption debugCmdOption(QStringList() << "c" << "Debug to command line");

    parser.addOption(loadFilesOption);
    parser.addOption(dummyDataOption);
    parser.addOption(debugCmdOption);

    parser.process(a);

    if (!parser.isSet(debugCmdOption))
    {
        // Install custom debug handler
        registerLumberjackDebugHandler();
    }

    qDebug() << "Lumberjack:" << getLumberjackVersion();

    MainWindow w;
    w.show();

    // Import data from specified files
    for (auto file : parser.values(loadFilesOption))
    {
        w.loadDataFromFile(file);
    }

    if (parser.isSet(dummyDataOption))
    {
        w.loadDummyData();
    }

    return a.exec();
}
