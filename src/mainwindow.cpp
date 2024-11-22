#include "mainwindow.h"
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    createActions();
    createMenus();
    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Water Quality Monitor"));
    resize(1024, 768);
}

void MainWindow::createActions()
{
    // TODO: Add actions
}

void MainWindow::createMenus()
{
    // TODO: Add menus
}
