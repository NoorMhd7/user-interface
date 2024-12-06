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
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *appTitle = new QLabel("Water Quality Monitor", this);
    appTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #f55ff3; padding: 10px;");
    headerLayout->addWidget(appTitle);

    // Language Selector
    QComboBox *languageSelector = new QComboBox(this);
    languageSelector->setFixedWidth(200);
    languageSelector->addItem("English");
    languageSelector->addItem("Spanish");
    languageSelector->setStyleSheet(R"(
        QComboBox {
           background-color: #4A5A76;
           color: #f55ff3;
           border: none;
           padding: 5px;
           border-radius: 4px;
       }
       QComboBox:hover {
           background-color: #3D485E;
       }
    
   )");
    headerLayout->addWidget(languageSelector, 0, Qt::AlignRight);
    mainLayout->addLayout(headerLayout);

    // Quick Filters
    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Filter by:", this);
    filterLabel->setStyleSheet("color: white;");
    filtersLayout->addWidget(filterLabel);

    QComboBox *timeRangeFilter = new QComboBox(this);
    timeRangeFilter->setFixedWidth(200);
    timeRangeFilter->addItem("Last Month");
    timeRangeFilter->addItem("Last Year");
    timeRangeFilter->setStyleSheet(R"(
       QComboBox {
           background-color: #4A5A76;
           color: #f55ff3;
           border: none;
           padding: 5px;
           border-radius: 4px;
       }
       QComboBox:hover {
           background-color: #3D485E;
       }
   )");
    filtersLayout->addWidget(timeRangeFilter);

    QComboBox *regionFilter = new QComboBox(this);
    regionFilter->setFixedWidth(200);
    regionFilter->addItem("All Regions");
    regionFilter->addItem("Region 1");
    regionFilter->addItem("Region 2");
    regionFilter->setStyleSheet(R"(
       QComboBox {
           background-color: #4A5A76;
           color: #f55ff3;
           border: none;
           padding: 5px;
           border-radius: 4px;
       }
       QComboBox:hover {
           background-color: #3D485E;
       }
   )");
    filtersLayout->addWidget(regionFilter);
    filtersLayout->addStretch();
    mainLayout->addLayout(filtersLayout);

    // Main Content
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("QScrollArea { background-color: #2F3A4F; border: none; }");
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background-color: #2F3A4F;");
    QGridLayout *contentLayout = new QGridLayout(contentWidget);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(10, 10, 10, 10);

    // Pollutant categories
    QStringList pollutantCategories = {"Pollutant Overview", "POPs", "Litter Indicators", "Fluorinated Compounds", "Data"};
    for (int i = 0; i < pollutantCategories.size(); ++i)
    {
        QFrame *pollutantCard = new QFrame(this);
        pollutantCard->setFrameShape(QFrame::StyledPanel);
        pollutantCard->setStyleSheet(R"(
           QFrame {
               background-color: #1E2638;
               border-radius: 4px;
               margin: 5px;
           }
       )");

        QVBoxLayout *pollutantCardLayout = new QVBoxLayout(pollutantCard);
        pollutantCardLayout->setContentsMargins(15, 15, 15, 15);
        pollutantCardLayout->setSpacing(10);

        QLabel *pollutantTitle = new QLabel(pollutantCategories[i], this);
        pollutantTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
        pollutantCardLayout->addWidget(pollutantTitle);

        QLabel *pollutantSummary = new QLabel("Average Level: 5.2 µg/L\nCompliance: Green", this);
        pollutantSummary->setStyleSheet("color: white;");
        pollutantCardLayout->addWidget(pollutantSummary);

        QPushButton *pollutantDetailsButton = new QPushButton("View Details", this);
        pollutantDetailsButton->setStyleSheet(R"(
           QPushButton {
               background-color: #4A5A76;
               color: white;
               border: none;
               padding: 5px;
               margin: 5px;
               border-radius: 2px;
               min-width: 100px;
           }
           QPushButton:hover {
               background-color: #3D485E;
           }
       )");
        pollutantCardLayout->addWidget(pollutantDetailsButton);
        contentLayout->addWidget(pollutantCard, i / 2, i % 2);

        // Connect buttons
        if (i == 0)
        {
            connect(pollutantDetailsButton, &QPushButton::clicked, this, &Dashboard::navigateToPollutantOverview);
        }
        else if (i == 1)
        {
            connect(pollutantDetailsButton, &QPushButton::clicked, this, &Dashboard::navigateToPOPs);
        }
        else if (i == 2)
        {
            connect(pollutantDetailsButton, &QPushButton::clicked, this, &Dashboard::navigateToLitterIndicators);
        }
        else if (i == 3)
        {
            connect(pollutantDetailsButton, &QPushButton::clicked, this, &Dashboard::navigateToCompound);
        }
        else if (i == 4)
        {
            connect(pollutantDetailsButton, &QPushButton::clicked, this, &Dashboard::navigateToData);
        }
    }

    contentWidget->setLayout(contentLayout);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Footer
    QHBoxLayout *footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(10);

    QStringList footerButtons = {"Help", "User Guide", "Credits"};
    for (const QString &buttonText : footerButtons)
    {
        QPushButton *button = new QPushButton(buttonText, this);
        button->setStyleSheet(R"(
           QPushButton {
               background-color: #4A5A76;
               color: white;
               border: none;
               padding: 8px 16px;
               border-radius: 4px;
               min-width: 100px;
           }
           QPushButton:hover {
               background-color: #3D485E;
           }
       )");
        footerLayout->addWidget(button);

        if (buttonText == "Help")
            connect(button, &QPushButton::clicked, this, &Dashboard::showHelp);
        else if (buttonText == "User Guide")
            connect(button, &QPushButton::clicked, this, &Dashboard::showUserGuide);
        else if (buttonText == "Credits")
            connect(button, &QPushButton::clicked, this, &Dashboard::showCredits);
    }

    footerLayout->addStretch();
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