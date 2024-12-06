#ifndef COMPOUND_H
#define COMPOUND_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class Compound : public QWidget
{
    Q_OBJECT

public:
    explicit Compound(QWidget *parent = nullptr);
    QChartView *chartView;

private:
    void setupUI();                  // Set up the user interface
    void loadChartData();            // Load data into the chart
    void updateComplianceStatus(QLineSeries *series); // Update compliance status for the series

    QVBoxLayout *mainLayout;         // Layout for arranging widgets
    QComboBox *locationFilter;       // Dropdown for location filter
    QComboBox *compoundFilter;       // Dropdown for compound filter
    QPushButton *refreshButton;      // Button to refresh the chart
    QLabel *complianceStatus;        // Label to display compliance status
};

#endif // COMPOUND_H
