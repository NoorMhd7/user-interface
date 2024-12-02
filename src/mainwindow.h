#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUi();
    void createActions();
    void createMenus();
    void setupSidebar();

    QStackedWidget *stackedWidget;
    QListWidget *sidebar;
private slots:
    void navigateToPage(int index);
    void showPollutantOverview();
    void showPOPs();
    void showLitterIndicators();
};

#endif // MAINWINDOW_H
