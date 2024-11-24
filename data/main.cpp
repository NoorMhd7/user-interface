// COMP2811 Coursework 2: application entry point

#include <QtWidgets>
#include "window.hpp"
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

  QuakeWindow window;
  window.show();

  return app.exec();
}
