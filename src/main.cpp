#include <QApplication>
#include "mainwindow.h"
#include <QtWidgets>
#include "data/window.hpp"
#include <QMainWindow>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load(":/translations/app_en.qm");
    app.installTranslator(&translator);

    MainWindow mainWindow;
    QuakeWindow window;
    mainWindow.show();
    return app.exec();
}
