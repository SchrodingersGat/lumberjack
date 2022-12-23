#include <QApplication>
#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

#include "PythonQt.h"

#include "lumberjack_debug.hpp"
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
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

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

    qDebug() << "Starting lumberjack application:" << LUMBERJACK_VERSION_STRING;

    MainWindow w;
    w.show();

    qDebug() << "Initializing python modules";

    PythonQt::init();
    PythonQtObjectPtr context = PythonQt::self()->getMainModule();

    /*
    PythonQt::setEnableThreadSupport(true);
    PythonQt_QtAll::init();


    */

    QObject::connect(PythonQt::self(), &PythonQt::pythonStdOut, [](const QString &v){ qDebug() << v; });
    QObject::connect(PythonQt::self(), &PythonQt::pythonStdErr, [](const QString &v){ qCritical() << v; });

    // Load dummy data if required
    if (parser.isSet(dummyDataOption))
    {
        qDebug() << "Loading dummy data";
        w.loadDummyData();
    }

    // Import data from specified files
    for (auto file : parser.values(loadFilesOption))
    {
        w.loadDataFromFile(file);
    }

    return a.exec();
}
