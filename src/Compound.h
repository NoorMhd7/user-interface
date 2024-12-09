#ifndef COMPOUND_H
#define COMPOUND_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "data/model.hpp"

class Compound : public QWidget
{
    Q_OBJECT

public:
    explicit Compound(QWidget *parent = nullptr);

private:
    void setupUI();                  // Set up the user interface
    void loadChartData();            // Load data into the chart
    void updateComplianceStatus(QLineSeries *series); // Update compliance status for the series

    QuakeModel model;
    QVBoxLayout *mainLayout;         // Layout for arranging widgets
    QChartView *chartView; // Widget to display the chart
    QComboBox *locationFilter;       // Dropdown for location filter
    QComboBox *compoundFilter;       // Dropdown for compound filter
    QPushButton *refreshButton;      // Button to refresh the chart
    QLabel *complianceStatus;        // Label to display compliance status
};

#endif // COMPOUND_H
