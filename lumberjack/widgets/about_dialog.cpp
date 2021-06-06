#include "about_dialog.hpp"
#include "lumberjack_version.hpp"


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);

    setWindowModality(Qt::ApplicationModal);

    setWindowTitle("Lumberjack v" + LUMBERJACK_VERSION_STRING);

    ui.softwareVersion->setText(LUMBERJACK_VERSION_STRING);
    ui.buildDate->setText(__DATE__);
    ui.qtVersion->setText(QT_VERSION_STR);

    ui.compiler->setText(COMPILER);
    ui.compilerVersion->setText(COMPILER_VERSION);
    ui.architecture->setText(ARCHITECTURE);

    ui.projectLInk->setTextFormat(Qt::RichText);
    ui.projectLInk->setOpenExternalLinks(true);
    ui.projectLInk->setText("<a href='https://github.com/SchrodingersGat/lumberjack/'>https://github.com/SchrodingersGat/lumberjack/</a>");

    connect(ui.closeButton, &QPushButton::released, this, &QDialog::reject);
}
