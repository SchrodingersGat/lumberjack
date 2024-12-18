#include <qwt_global.h>

#include "about_dialog.hpp"
#include "lumberjack_version.hpp"


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);

    setWindowModality(Qt::ApplicationModal);

    setWindowTitle("Lumberjack v" + getLumberjackVersion());

    ui.softwareVersion->setText(getLumberjackVersion());
    ui.buildDate->setText(__DATE__);

    QString compileInfo = QString(COMPILER);

    compileInfo += " ";
    compileInfo += QString(COMPILER_VERSION);
    compileInfo += " ";
    compileInfo += QString(ARCHITECTURE);

    ui.compiler->setText(compileInfo);

    ui.projectLInk->setTextFormat(Qt::RichText);
    ui.projectLInk->setOpenExternalLinks(true);
    ui.projectLInk->setText("<a href='https://github.com/SchrodingersGat/lumberjack/'>https://github.com/SchrodingersGat/lumberjack/</a>");

    ui.qtVersion->setTextFormat(Qt::RichText);
    ui.qtVersion->setOpenExternalLinks(true);
    ui.qtVersion->setText(
        QString("<a href='https://www.qt.io/'>") +
        QString(QT_VERSION_STR) +
        QString("</a>")
    );

    ui.qwtVersion->setTextFormat(Qt::RichText);
    ui.qwtVersion->setOpenExternalLinks(true);
    ui.qwtVersion->setText(
        QString("<a href='https://qwt.sourceforge.io/'>") +
        QString(QWT_VERSION_STR) +
        QString("</a>")
    );

    connect(ui.closeButton, &QPushButton::released, this, &QDialog::reject);
}
