#include "LitterIndicators.h"
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QLabel>

LitterIndicators::LitterIndicators(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadChartData();
}

void LitterIndicators::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Location filter (ComboBox)
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All Locations");
    locationFilter->addItem("Location A");
    locationFilter->addItem("Location B");
    layout->addWidget(locationFilter);

    // Litter Type filter (ComboBox)
    litterTypeFilter = new QComboBox(this);
    litterTypeFilter->addItem("All Types");
    litterTypeFilter->addItem("Plastic");
    litterTypeFilter->addItem("Paper");
    litterTypeFilter->addItem("Metal");
    layout->addWidget(litterTypeFilter);

    // Refresh button
    refreshButton = new QPushButton("Refresh", this);
    layout->addWidget(refreshButton);

    // Connect refresh button signal to loadChartData slot
    connect(refreshButton, &QPushButton::clicked, this, &LitterIndicators::loadChartData);

    // Chart view
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);

    // Compliance overview section
    complianceOverview = new QLabel(this);
    // Show different colours depending on the values ( should i add a graph here or not?)
    complianceOverview->setText("Compliance Overview: All monitored locations comply with standards.");
    complianceOverview->setStyleSheet("font-weight: bold; color: green; margin-top: 10px;");
    layout->addWidget(complianceOverview);

    setLayout(layout);
}

void LitterIndicators::loadChartData()
{
    // Create a bar set for the chart
    QBarSet *barSet = new QBarSet("Litter Counts");
    *barSet << 5 << 10 << 8; // Example data

    // Create a bar series and add the bar set
    QBarSeries *barSeries = new QBarSeries();
    barSeries->append(barSet);

    // Create a bar chart
    QChart *chart = new QChart();
    chart->addSeries(barSeries);
    chart->setTitle("Environmental Litter Indicators");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set categories for the X-axis
    QStringList categories;
    categories << "Location A" << "Location B" << "Location C";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->setAxisX(axisX, barSeries);

    // Create default Y-axis
    chart->createDefaultAxes();

    // Assign the chart to the chart view
    chartView->setChart(chart);
}