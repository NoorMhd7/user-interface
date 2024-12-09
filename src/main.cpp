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
#include "data/config.h"
#include "data/data_processors.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DataProcessor processor;
    processor.processData();

    MainWindow mainWindow;
    QuakeWindow window;
    mainWindow.show();
    return app.exec();
}
