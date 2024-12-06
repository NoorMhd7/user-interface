#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QSplitter>
#include "PollutantOverview.h"
#include "LitterIndicators.h"
#include "dashboard.h"
#include "data/window.hpp"
#include "pop.h"
#include "Compound.h"
#include <QFont>

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
    this->setStyleSheet("QMainWindow { background-color: #2F3A4F; }");

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    // Sidebar container with title
    QWidget *sidebarContainer = new QWidget(this);
    sidebarContainer->setStyleSheet(R"(
        QWidget {
            background-color: #2F3A4F;
            border-right: 1px solid #1E2638;
            margin-right: 2px;  /* Space for shadow */
        }
    )");

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setBlurRadius(10);
    shadowEffect->setXOffset(2);
    shadowEffect->setYOffset(0);
    shadowEffect->setColor(QColor(0, 0, 0, 80)); // Semi-transparent black
    sidebarContainer->setGraphicsEffect(shadowEffect);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    // Sidebar title
    QLabel *sidebarTitle = new QLabel("Navigation", sidebarContainer);
    sidebarTitle->setAlignment(Qt::AlignCenter);
    sidebarTitle->setStyleSheet(R"(
        QLabel {
            background-color: #1E2638;
            color: #f55ff3;
            font-size: 16px;
            font-weight: bold;
            font-family: Helvetica, Arial, sans-serif;
            padding: 10px;
        }
    )");

    // Sidebar setup
    sidebar->setFixedWidth(225);

    // Add items and set text alignment for each
    QListWidgetItem *dashboardItem = new QListWidgetItem("Dashboard", sidebar);
    dashboardItem->setTextAlignment(Qt::AlignCenter);

    QListWidgetItem *pollutantO = new QListWidgetItem("Pollutant Overview", sidebar);
    pollutantO->setTextAlignment(Qt::AlignCenter);

    QListWidgetItem *OrganicItem = new QListWidgetItem("Organic Pollutants", sidebar);
    OrganicItem->setTextAlignment(Qt::AlignCenter);

    QListWidgetItem *LitterInd = new QListWidgetItem("Litter Indicators", sidebar);
    LitterInd->setTextAlignment(Qt::AlignCenter);

    QListWidgetItem *FCompound = new QListWidgetItem("Fluorinated Compounds", sidebar);
    FCompound->setTextAlignment(Qt::AlignCenter);

    QListWidgetItem *dataItem = new QListWidgetItem("Data", sidebar);
    dataItem->setTextAlignment(Qt::AlignCenter);

    QFont sidebarFont("Helvetica", 14, QFont::Bold); // Font: Helvetica, Size: 16, Bold
    sidebar->setFont(sidebarFont);

    // Style the sidebar
    sidebar->setStyleSheet(R"(
    QListWidget {
        background-color: #2F3A4F;
        color: #FFFFFF;
        border: none;
    }
    QListWidget::item {
        padding: 15px;
        color: #FFFFFF;
    }
    QListWidget::item:selected {
        background-color: #4A5A76;
        color: #FFFFFF;
    }
    QListWidget::item:hover {
        background-color: #3D485E;
    }
)");

    // Add title and sidebar to the layout
    sidebarLayout->addWidget(sidebarTitle);
    sidebarLayout->addWidget(sidebar);
    sidebarLayout->addStretch(); // Push the sidebar content upwards

    // Add sidebarContainer to the splitter
    splitter->addWidget(sidebarContainer);

    // Dashboard
    Dashboard *dashboardPage = new Dashboard();
    stackedWidget->addWidget(dashboardPage);

    // Pollutant Overview Page
    PollutantOverview *pollutantOverviewPage = new PollutantOverview();
    stackedWidget->addWidget(pollutantOverviewPage);

    // POPs Page
    Pop *popsPage = new Pop();
    stackedWidget->addWidget(popsPage);

    // Litter Indicators Page
    LitterIndicators *litterIndicatorsPage = new LitterIndicators();
    stackedWidget->addWidget(litterIndicatorsPage);

    // Compound Page
    Compound *CompoundPage = new Compound();
    stackedWidget->addWidget(CompoundPage);

    // Data Page
    QuakeWindow *quakeWindow = new QuakeWindow();
    stackedWidget->addWidget(quakeWindow);

    // Add stackedWidget to the splitter
    splitter->addWidget(stackedWidget);
    splitter->setStretchFactor(1, 1); // Ensure content takes the rest of the space

    // Set sidebarContainer to stretch fully
    splitter->setStretchFactor(0, 0); // Ensure sidebar maintains its fixed width

    // Remove splitter margins and spacing
    splitter->setHandleWidth(0);
    splitter->setStyleSheet("QSplitter::handle { background: none; }");

    setCentralWidget(splitter);

    // Signal-slot connection for navigation
    connect(sidebar, &QListWidget::currentRowChanged, this, &MainWindow::navigateToPage);
    sidebar->setCurrentRow(0);

    // Connect dashboard signals to main window slots
    connect(dashboardPage, &Dashboard::navigateToPollutantOverview, this, &MainWindow::showPollutantOverview);
    connect(dashboardPage, &Dashboard::navigateToPOPs, this, &MainWindow::showPOPs);
    connect(dashboardPage, &Dashboard::navigateToLitterIndicators, this, &MainWindow::showLitterIndicators);
    connect(dashboardPage, &Dashboard::navigateToFluorinatedCompounds, this, &MainWindow::showFluorinatedCompounds); 
}

void MainWindow::setupSidebar()
{
}

void MainWindow::navigateToPage(int index)
{
    stackedWidget->setCurrentIndex(index);
}

void MainWindow::showPollutantOverview()
{
    stackedWidget->setCurrentIndex(1); // Navigate to Pollutant Overview Page
    updateSidebarSelection(1);         // Update sidebar selection
}

void MainWindow::showPOPs()
{
    stackedWidget->setCurrentIndex(2); // Navigate to POPs Page
    updateSidebarSelection(2);         // Update sidebar selection
}

void MainWindow::showLitterIndicators()
{
    stackedWidget->setCurrentIndex(3); // Navigate to Litter Indicators Page
    updateSidebarSelection(3);         // Update sidebar selection
}

void MainWindow::showFluorinatedCompounds()
{
    stackedWidget->setCurrentIndex(4); // Navigate to Fluorinated Compounds Page
    updateSidebarSelection(4);         // Update sidebar selection
}

void MainWindow::updateSidebarSelection(int index)
{
    sidebar->setCurrentRow(index);
}