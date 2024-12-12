#ifndef POLLUTANT_OVERVIEW_H
#define POLLUTANT_OVERVIEW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "data/model.hpp"

class PollutantOverview : public QWidget
{
    Q_OBJECT

public:
    explicit PollutantOverview(QWidget *parent = nullptr);
    QStringList getCompoundBoxTexts() const;
    QStringList getCompoundBoxColors() const;

private:
    // Method to set up the user interface
    void setupUI(); 

    // Method to load chart data
    void loadChartData(); 

    // Data model instance
    QuakeModel model; 

    // Main layout for arranging widgets
    QVBoxLayout *mainLayout;

    // Layout for litter boxes
    QHBoxLayout *litterBoxLayout;

    // Widget to display the chart
    QChartView *chartView;

    // Search input for litter types
    QLineEdit *searchInput;

    // Button to refresh the chart
    QPushButton *searchButton;

    // Label to display compliance status
    QLabel *complianceStatus;

    // QLabel instances for displaying compound names
    QLabel *litterName;
    
    // List to store the compound boxes for display
    QList<QLabel*> litterBoxes;
};

#endif // POLLUTANT_OVERVIEW_H