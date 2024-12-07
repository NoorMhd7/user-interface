#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QTranslator>
#include <QtCharts/QChartView>
#include "Compound.h"
#include "pop.h"
#include "PollutantOverview.h"
#include "LitterIndicators.h"

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

signals:
    void navigateToPollutantOverview();
    void navigateToPOPs();
    void navigateToLitterIndicators();
    void navigateToFluorinatedCompounds();

private slots:
    void showHelp();
    void showUserGuide();
    void showCredits();
    void changeLanguage(const QString &language);

private:
    QTranslator translator;
    QLabel *appTitle;
    QComboBox *languageSelector;
    void retranslateUi();

    // Instances of individual pages
    Compound *compoundPage;
    Pop *popPage;
    PollutantOverview *pollutantOverviewPage;
    LitterIndicators *litterIndicatorsPage;

    void addChartWithButton(QGridLayout *layout, QChartView *chartView, const QString &title, const QString &buttonText, int row, int col, void (Dashboard::*slot)())
    {
        if (!chartView) {
            qDebug() << "Error: chartView is null";
            return;  // Don't proceed if chartView is null
        }
        QVBoxLayout *chartLayout = new QVBoxLayout();
        
        QLabel *chartTitle = new QLabel(title, this);
        chartTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #f55ff3; padding: 5px;");
        chartLayout->addWidget(chartTitle);

        chartView->setMinimumSize(60, 60);
        chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        chartLayout->addWidget(chartView);

        QPushButton *detailsButton = new QPushButton(buttonText, this);
        detailsButton->setStyleSheet(R"(
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
        connect(detailsButton, &QPushButton::clicked, this, slot);
        chartLayout->addWidget(detailsButton);

        QWidget *chartWidget = new QWidget(this);
        chartWidget->setLayout(chartLayout);
        layout->addWidget(chartWidget, row, col);
        layout->setRowStretch(row, 1);
        layout->setColumnStretch(col, 1);
    }
};

#endif // DASHBOARD_H