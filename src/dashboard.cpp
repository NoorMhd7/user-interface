#include "dashboard.h"
#include "LitterIndicators.h"
#include "pop.h"
#include "Compound.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QScrollArea>
#include <QGridLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <QDebug>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *appTitle = new QLabel(tr("Water Quality Monitor"), this);
    appTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #f55ff3; padding: 10px;");
    headerLayout->addStretch();
    headerLayout->addWidget(appTitle, 0, Qt::AlignCenter);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // Main Content
    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("QScrollArea { background-color: #2F3A4F; border: none; }");
    contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background-color: #2F3A4F;");
    contentLayout = new QGridLayout(contentWidget);
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(10, 10, 10, 10);

    // Create instances of LitterIndicators, Pop, and Compound to get the description text
    pollutantOverviewPage = new PollutantOverview();
    litterIndicatorsPage = new LitterIndicators();
    popPage = new Pop();
    compoundPage = new Compound();

    // Ensure the instances are created successfully
    if (!litterIndicatorsPage || !popPage || !compoundPage)
    {
        QMessageBox::critical(this, tr("Initialization Error"), tr("Failed to create page instances."));
        return;
    }

    // Add pollutant cards
    addPollutantCard(tr("Pollutant Overview"),
                     pollutantOverviewPage->getConcentrationText(), // Use the concentration text here
                     "",
                     &Dashboard::navigateToPollutantOverview);
    addPollutantCard(tr("POPs"), "", popPage->getSafetyAnalysisText(), &Dashboard::navigateToPOPs);
    addPollutantCard(tr("Litter Indicators"), "", litterIndicatorsPage->getDescriptionText(), &Dashboard::navigateToLitterIndicators);
    addPollutantCard(tr("Fluorinated Compounds"), tr("Compound Concentrations Over Time for All Locations\n"), tr("Compliance Indicators use traffic-light colours to show safety levels:\n"), &Dashboard::navigateToCompound);
    addPollutantCard(tr("Data"), tr("Average Level: 643.0995 \nTotal Measurements: 144193 \nBelow Detection Limit: 49929"), "", &Dashboard::navigateToData);

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
        footerLayout->addStretch();
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

    // Adjust layout based on initial size
    adjustCardLayout();
}

void Dashboard::addPollutantCard(const QString &title, const QString &summary, const QString &description, void (Dashboard::*slot)())
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

    QLabel *pollutantTitle = new QLabel(title, this);
    pollutantTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    pollutantCardLayout->addWidget(pollutantTitle);

    if (!summary.isEmpty())
    {
        QLabel *pollutantSummary = new QLabel(summary, this);
        pollutantSummary->setStyleSheet("color: white;");
        pollutantCardLayout->addWidget(pollutantSummary);
    }

    QString defaultDescription = "";
    QLabel *pollutantDescription = new QLabel(description.isEmpty() ? defaultDescription : description, this);
    pollutantDescription->setStyleSheet("color: white;");
    pollutantCardLayout->addWidget(pollutantDescription);

    // Add compound boxes if the title is "Fluorinated Compounds"
    if (title == tr("Fluorinated Compounds"))
    {
        QHBoxLayout *compoundBoxLayout = new QHBoxLayout();
        QStringList boxTexts = compoundPage->getCompoundBoxTexts();
        QStringList boxColors = compoundPage->getCompoundBoxColors();
        for (int i = 0; i < boxTexts.size(); ++i)
        {
            QLabel *box = new QLabel(boxTexts[i], this);
            box->setFixedSize(100, 100);
            box->setAlignment(Qt::AlignCenter);
            box->setStyleSheet(QString("background-color: %1; color: white; font-size: 16px; border-radius: 15px;").arg(boxColors[i]));
            compoundBoxLayout->addWidget(box);
        }
        pollutantCardLayout->addLayout(compoundBoxLayout);
    }

    QPushButton *pollutantDetailsButton = new QPushButton(tr("View Details"), this);
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
    connect(pollutantDetailsButton, &QPushButton::clicked, this, slot);

    cards.append(pollutantCard);
}

void Dashboard::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustCardLayout();
}

void Dashboard::adjustCardLayout()
{
    if (cards.isEmpty())
        return;

    int minCardWidth = 500;
    int spacing = 10;
    int availableWidth = scrollArea->viewport()->width() -
                         contentLayout->contentsMargins().left() -
                         contentLayout->contentsMargins().right();

    // Calculate number of columns (minimum 1)
    int columns = std::max(1, availableWidth / (minCardWidth + spacing));

    // Only reorganize if number of columns has changed
    static int previousColumns = 0;
    if (previousColumns == columns && !contentLayout->isEmpty())
    {
        return;
    }
    previousColumns = columns;

    // Calculate optimal card width
    int cardWidth = (availableWidth - (columns - 1) * spacing) / columns;

    // Temporarily hide widget to prevent flickering
    contentWidget->setVisible(false);

    // Remove items from layout but don't delete widgets
    while (contentLayout->count())
    {
        QLayoutItem *item = contentLayout->takeAt(0);
        if (item->widget())
        {
            item->widget()->hide();
        }
        delete item;
    }

    // Reset layout stretches
    for (int i = 0; i < contentLayout->columnCount(); ++i)
    {
        contentLayout->setColumnStretch(i, 0);
    }
    for (int i = 0; i < contentLayout->rowCount(); ++i)
    {
        contentLayout->setRowStretch(i, 0);
    }

    // Add cards back to layout
    int row = 0;
    int col = 0;
    for (QFrame *card : cards)
    {
        if (card)
        {
            card->setMinimumWidth(minCardWidth);
            card->setMaximumWidth(cardWidth);
            contentLayout->addWidget(card, row, col);
            card->show();

            col++;
            if (col >= columns)
            {
                col = 0;
                row++;
            }
        }
    }

    // Set equal stretch for columns and rows
    for (int c = 0; c < columns; ++c)
    {
        contentLayout->setColumnStretch(c, 1);
    }
    for (int r = 0; r <= row; ++r)
    {
        contentLayout->setRowStretch(r, 1);
    }

    // Make widget visible again
    contentWidget->setVisible(true);
}

void Dashboard::showHelp()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Help"));
    msgBox.setText(tr("This application helps you monitor water quality and pollutants. Click on View Details to find out more on each page."));
    msgBox.setStyleSheet("QMessageBox { background-color: #2F3A4F; color: white; } QLabel { color: white; } QPushButton { background-color: #4A5A76; color: white; }");
    msgBox.exec();
}

void Dashboard::showUserGuide()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("User Guide"));
    msgBox.setText(tr("User Guide:\n\n1. View summarised data for each page on the Dashboard cards.\n2. Click on 'View Details' buttons on each card to navigate to detailed pages.\n3. Use the 'Help' button for assistance and the 'Credits' button to view data sources."));
    msgBox.setStyleSheet("QMessageBox { background-color: #2F3A4F; color: white; } QLabel { color: white; } QPushButton { background-color: #4A5A76; color: white; }");
    msgBox.exec();
}

void Dashboard::showCredits()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Credits"));
    msgBox.setText(tr("Credits for data sources:\nEnvironment Food & Rural Affairs, D. Download open water quality archive datasets. Available at: https://environment.data.gov.uk/water-quality/view/download (Accessed: 11 December 2024). "));
    msgBox.setStyleSheet("QMessageBox { background-color: #2F3A4F; color: white; } QLabel { color: white; } QPushButton { background-color: #4A5A76; color: white; }");
    msgBox.exec();
}