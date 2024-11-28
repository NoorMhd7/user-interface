#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include "examplepage.h"
#include "dashboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), stackedWidget(new QStackedWidget), sidebar(new QListWidget)
{
    setupUi();
    setupSidebar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Water Quality Monitor"));
    resize(1024, 768);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    // Sidebar setup
    sidebar->setFixedWidth(200);
    sidebar->addItem("Dashboard");
    sidebar->addItem("Example");

    // Add sidebar to the splitter
    splitter->addWidget(sidebar);

    // Dashboard
    Dashboard *dashboardPage = new Dashboard();
    stackedWidget->addWidget(dashboardPage);

    // Examplepage
    ExamplePage *examplePage = new ExamplePage();
    stackedWidget->addWidget(examplePage);

    // Add stackedWidget to the splitter
    splitter->addWidget(stackedWidget);
    splitter->setStretchFactor(1, 1); // Ensure content takes the rest of the space

    // Remove splitter margins and spacing
    splitter->setHandleWidth(0);
    splitter->setStyleSheet("QSplitter::handle { background: none; }");

    setCentralWidget(splitter);

    connect(sidebar, &QListWidget::currentRowChanged, this, &MainWindow::navigateToPage);
    sidebar->setCurrentRow(0);
}

void MainWindow::setupSidebar()
{
}

void MainWindow::navigateToPage(int index)
{
    stackedWidget->setCurrentIndex(index);
}
