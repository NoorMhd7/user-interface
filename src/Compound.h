#ifndef COMPOUND_H
#define COMPOUND_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QHBoxLayout> // For layout of the compound boxes
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "data/model.hpp"

class Compound : public QWidget
{
    Q_OBJECT

public:
    explicit Compound(QWidget *parent = nullptr);
    QStringList getCompoundBoxTexts() const;
    QStringList getCompoundBoxColors() const;

private:
    // Method to set up the user interface
    void setupUI(); 

    // Method to load chart data
    void loadChartData(); 

    // Method to update compliance status for the chart series
    void updateComplianceStatus(QLineSeries *series);

    // Method to update the compound boxes based on selected values
    void updateCompoundBoxes(double pfosValue, double pfhxsBValue, double hfpoDAValue, double pfbsValue);

    // Data model instance
    QuakeModel model; 

    // Main layout for arranging widgets
    QVBoxLayout *mainLayout;

    // Widget to display the chart
    QChartView *chartView;

    // Dropdowns for location and compound filters
    QComboBox *locationFilter;       
    QComboBox *compoundFilter;

    // Button to refresh the chart
    QPushButton *refreshButton;

    // Label to display compliance status
    QLabel *complianceStatus;

    // QLabel instances for displaying compound names
    QLabel *pfosBox;
    QLabel *pfhxsBBox;
    QLabel *hfpoDABBox;
    QLabel *pfbsBox;
    QLabel *genericBox; // Optional generic box
    
    // List to store the compound boxes for display
    QList<QLabel*> compoundBoxes;
};

#endif // COMPOUND_H