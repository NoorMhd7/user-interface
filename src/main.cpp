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
    QLocale::setDefault(QLocale("en_EN"));  // Set the desired locale here
    
    QApplication app(argc, argv);

    QTranslator translator;
    if (translator.load("water-quality-qt_en.qm", ":/i18n")) {
        app.installTranslator(&translator);
    }

    DataProcessor processor;
    processor.processData();

    MainWindow mainWindow;
    QuakeWindow window;
    mainWindow.show();
    return app.exec();
}
