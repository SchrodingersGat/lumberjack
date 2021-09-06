#ifndef ABOUT_DIALOG_HPP
#define ABOUT_DIALOG_HPP

#include <QDialog>

#include "ui_about_dialog.h"


class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = nullptr);

protected:
    Ui::about_dialog ui;
};


#endif // ABOUT_DIALOG_HPP
