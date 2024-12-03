#include "pop.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QtCharts>
#include <QFont>

Pop::Pop(QWidget *parent)
    : QWidget(parent)
{
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel("Organic Pollutants Line Chart", this);
    QFont titleFont("Arial", 24, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Location Dropdown
    locationDropdown = new QComboBox(this);
    locationDropdown->setFixedWidth(200);
    locationDropdown->addItem("Select Location");
    // Add sample locations
    for (int i = 1; i <= 10; i++)
    {
        locationDropdown->addItem(QString("Location %1").arg(i));
    }
    mainLayout->addWidget(locationDropdown, 0, Qt::AlignLeft);

    // Create chart
    auto chart = new QChart();
    chart->setTitle("Pollutant Concentrations Over Time");
    chart->setAnimationOptions(QChart::AllAnimations);

    // Create sample data
    auto series = new QLineSeries();
    series->setName("Sample Pollutant");

    // Use random numbers for sample data
    QRandomGenerator *rng = QRandomGenerator::global();
    for (int j = 0; j < 10; j++)
    {
        series->append(j, rng->bounded(100));
    }
    chart->addSeries(series);

    // Customize axes
    auto axisX = new QValueAxis;
    axisX->setTitleText("Time Period");
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QValueAxis;
    axisY->setTitleText("Concentration (μg/L)");
    axisY->setRange(0, 100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(500);
    mainLayout->addWidget(chartView, 1);

    // Connect dropdown signal
    connect(locationDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Pop::onLocationChanged);
}

void Pop::onLocationChanged(int index)
{
    if (index > 0)
    {
        // Update chart data based on selected location
        // Implement your data update logic here
    }
}