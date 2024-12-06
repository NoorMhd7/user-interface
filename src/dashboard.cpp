#include "dashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QGridLayout>
#include <QMessageBox>
#include <QApplication>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent), appTitle(new QLabel(this)), languageSelector(new QComboBox(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    appTitle->setText(tr("Water Quality Monitor"));
    appTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #f55ff3; padding: 10px;");
    headerLayout->addWidget(appTitle);

    // Language Selector
    languageSelector->setFixedWidth(200);
    languageSelector->addItem(tr("English"), "en");
    languageSelector->addItem(tr("Spanish"), "es");
    languageSelector->addItem(tr("French"), "fr");
    languageSelector->addItem(tr("Chinese"), "zh");
    languageSelector->addItem(tr("Hindi"), "hi");
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
    connect(languageSelector, &QComboBox::currentTextChanged, this, &Dashboard::changeLanguage);
    headerLayout->addWidget(languageSelector, 0, Qt::AlignRight);
    mainLayout->addLayout(headerLayout);

    // Quick Filters
    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel(tr("Filter by:"), this);
    filterLabel->setStyleSheet("color: white;");
    filtersLayout->addWidget(filterLabel);

    QComboBox *timeRangeFilter = new QComboBox(this);
    timeRangeFilter->setFixedWidth(200);
    timeRangeFilter->addItem(tr("Last Month"));
    timeRangeFilter->addItem(tr("Last Year"));
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
    regionFilter->addItem(tr("All Regions"));
    regionFilter->addItem(tr("Region 1"));
    regionFilter->addItem(tr("Region 2"));
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

    // Create instances of individual pages
    compoundPage = new Compound(this);
    popPage = new Pop(this);
    pollutantOverviewPage = new PollutantOverview(this);
    litterIndicatorsPage = new LitterIndicators(this);

    // Add charts and "View Details" buttons from individual pages to the dashboard
    addChartWithButton(contentLayout, compoundPage->chartView, tr("Fluorinated Compounds"), tr("View Details"), 0, 0, &Dashboard::navigateToFluorinatedCompounds);
    addChartWithButton(contentLayout, popPage->chartView, tr("Persistent Organic Pollutants (POPs)"), tr("View Details"), 0, 1, &Dashboard::navigateToPOPs);
    addChartWithButton(contentLayout, pollutantOverviewPage->chartView, tr("Pollutant Overview"), tr("View Details"), 1, 0, &Dashboard::navigateToPollutantOverview);
    addChartWithButton(contentLayout, litterIndicatorsPage->chartView, tr("Litter Indicators"), tr("View Details"), 1, 1, &Dashboard::navigateToLitterIndicators);

    contentWidget->setLayout(contentLayout);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Footer
    QHBoxLayout *footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(10);

    QStringList footerButtons = {tr("Help"), tr("User Guide"), tr("Credits")};
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

        if (buttonText == tr("Help"))
            connect(button, &QPushButton::clicked, this, &Dashboard::showHelp);
        else if (buttonText == tr("User Guide"))
            connect(button, &QPushButton::clicked, this, &Dashboard::showUserGuide);
        else if (buttonText == tr("Credits"))
            connect(button, &QPushButton::clicked, this, &Dashboard::showCredits);
    }

    footerLayout->addStretch();
    mainLayout->addLayout(footerLayout);
    setLayout(mainLayout);
}

void Dashboard::showHelp()
{
    QMessageBox::information(this, tr("Help"), tr("This is the help information."));
}

void Dashboard::showUserGuide()
{
    QMessageBox::information(this, tr("User Guide"), tr("This is the user guide information."));
}

void Dashboard::showCredits()
{
    QMessageBox::information(this, tr("Credits"), tr("These are the credits for data sources."));
}

void Dashboard::changeLanguage(const QString &language)
{
    QString langCode = languageSelector->currentData().toString();
    if (translator.load(":/translations/app_" + langCode + ".qm")) {
        qApp->installTranslator(&translator);
        retranslateUi();
    }
}

void Dashboard::retranslateUi()
{
    appTitle->setText(tr("Water Quality Monitor"));
    languageSelector->setItemText(0, tr("English"));
    languageSelector->setItemText(1, tr("Spanish"));
    languageSelector->setItemText(2, tr("French"));
    languageSelector->setItemText(3, tr("Chinese"));
    languageSelector->setItemText(4, tr("Hindi"));
    // Update other UI elements similarly
}