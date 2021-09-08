#include <QApplication>

#include "lumberjack_version.hpp"

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Configure application properties
    a.setApplicationName("lumberjack");
    a.setApplicationDisplayName("lumberjack");
    a.setApplicationVersion(LUMBERJACK_VERSION_STRING);

    MainWindow w;
    w.show();

    return a.exec();
}
