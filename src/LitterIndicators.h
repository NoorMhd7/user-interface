#ifndef LitterIndicators_H2
#define LitterIndicators_H2

#include <QWidget>
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "data/model.hpp"

class LitterIndicators : public QWidget
{
    Q_OBJECT

public:
    explicit LitterIndicators(QWidget *parent = nullptr);

private:
    void setupUI();
    void loadChartData();

    QuakeModel model;
    QVBoxLayout *mainLayout; // Layout for arranging widgets
    QChartView *chartView;   // Widget to display the chart
    QComboBox *locationFilter;
    QComboBox *materialTypeFilter;
    QPushButton *refreshButton;
    QLabel *complianceOverview; 
};

#endif // EXAMPLEPAGE_H2
