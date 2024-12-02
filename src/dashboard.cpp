#include "dashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QScrollArea>
#include <QGridLayout>
#include <QMessageBox>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *appTitle = new QLabel("Water Quality Monitor", this);
    appTitle->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(appTitle);

    // Language Selector
    QComboBox *languageSelector = new QComboBox(this);
    languageSelector->addItem("English");
    languageSelector->addItem("Spanish");
    headerLayout->addWidget(languageSelector);

    mainLayout->addLayout(headerLayout);

    // Quick Filters
    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Filter by:", this);
    filtersLayout->addWidget(filterLabel);
    QComboBox *timeRangeFilter = new QComboBox(this);
    timeRangeFilter->addItem("Last Month");
    timeRangeFilter->addItem("Last Year");
    filtersLayout->addWidget(timeRangeFilter);
    QComboBox *regionFilter = new QComboBox(this);
    regionFilter->addItem("All Regions");
    regionFilter->addItem("Region 1");
    regionFilter->addItem("Region 2");
    filtersLayout->addWidget(regionFilter);
    mainLayout->addLayout(filtersLayout);

    // Main Content
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *contentWidget = new QWidget(this);
    QGridLayout *contentLayout = new QGridLayout(contentWidget);

    // Pollutant categories
    QStringList pollutantCategories = {"Pollutant Overview", "POPs", "Litter Indicators", "Fluorinated Compounds", "Compliance"};
    for (int i = 0; i < pollutantCategories.size(); ++i) {
        QFrame *pollutantCard = new QFrame(this);
        pollutantCard->setFrameShape(QFrame::StyledPanel);
        QVBoxLayout *pollutantCardLayout = new QVBoxLayout(pollutantCard);
        QLabel *pollutantTitle = new QLabel(pollutantCategories[i], this);
        pollutantTitle->setStyleSheet("font-size: 18px; font-weight: bold;");
        pollutantCardLayout->addWidget(pollutantTitle);
        QLabel *pollutantSummary = new QLabel("Average Level: 5.2 µg/L\nCompliance: Green", this);
        pollutantCardLayout->addWidget(pollutantSummary);
        QPushButton *pollutantDetailsButton = new QPushButton("View Details", this);
        pollutantCardLayout->addWidget(pollutantDetailsButton);
        contentLayout->addWidget(pollutantCard, i / 2, i % 2); // Arrange in a grid
    }

    contentWidget->setLayout(contentLayout);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

        // Footer
    QHBoxLayout *footerLayout = new QHBoxLayout();
    QPushButton *helpButton = new QPushButton("Help", this);
    QPushButton *userGuideButton = new QPushButton("User Guide", this);
    QPushButton *creditsButton = new QPushButton("Credits", this);

    connect(helpButton, &QPushButton::clicked, this, &Dashboard::showHelp);
    connect(userGuideButton, &QPushButton::clicked, this, &Dashboard::showUserGuide);
    connect(creditsButton, &QPushButton::clicked, this, &Dashboard::showCredits);

    footerLayout->addWidget(helpButton);
    footerLayout->addWidget(userGuideButton);
    footerLayout->addWidget(creditsButton);
    mainLayout->addLayout(footerLayout);

    setLayout(mainLayout);
}

void Dashboard::showHelp()
{
    QMessageBox::information(this, "Help", "This is the help information.");
}

void Dashboard::showUserGuide()
{
    QMessageBox::information(this, "User Guide", "This is the user guide information.");
}

void Dashboard::showCredits()
{
    QMessageBox::information(this, "Credits", "These are the credits for data sources.");
}