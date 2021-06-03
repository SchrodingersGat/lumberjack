#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initStatusBar();
    initSignalsSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSignalsSlots()
{

}


void MainWindow::initStatusBar()
{
    t_pos.setText("t");
    y_pos.setText("y");

    ui->statusbar->showMessage("lumberjack");

    ui->statusbar->addPermanentWidget(&t_pos);
    ui->statusbar->addPermanentWidget(&y_pos);
}
