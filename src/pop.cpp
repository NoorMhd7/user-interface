#include "pop.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QtCharts>
#include <QFont>
#include <QRandomGenerator>

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
    titleLabel->setStyleSheet("color: #f55ff3;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Location Dropdown
    locationDropdown = new QComboBox(this);
    locationDropdown->setFixedWidth(200);
    locationDropdown->addItem("Select Location");
    locationDropdown->setStyleSheet(R"(
       QComboBox {
           background-color: #4A5A76;
           color: #fffff;
           border: none;
           padding: 5px;
           border-radius: 4px;
       }
       QComboBox:hover {
           background-color: #3D485E;
       }
       QComboBox::drop-down {
           border: none;
       }
       QComboBox::down-arrow {
           width: 12px;
           height: 12px;
           margin-right: 5px;
           image: none;
           border-left: 3px solid #f55ff3;
           border-bottom: 3px solid #f55ff3;
           transform: rotate(-45deg);
       }
   )");

    // Add sample locations
    for (int i = 1; i <= 10; i++)
    {
        locationDropdown->addItem(QString("Location %1").arg(i));
    }
    mainLayout->addWidget(locationDropdown, 0, Qt::AlignLeft);

    // Create chart with dark theme
    auto chart = new QChart();
    chart->setTitle("Pollutant Concentrations Over Time");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setBackgroundBrush(QColor("#2F3A4F"));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelBrush(QBrush(Qt::white));

    // Create sample data with pink line
    auto series = new QLineSeries();
    series->setName("POP and PCB Pollutants");
    series->setPen(QPen(QColor("#f55ff3"), 2));

    // Use random numbers for sample data
    QRandomGenerator *rng = QRandomGenerator::global();
    for (int j = 0; j < 10; j++)
    {
        series->append(j, rng->bounded(100));
    }
    chart->addSeries(series);

    // Customize axes with white text
    auto axisX = new QValueAxis;
    axisX->setTitleText("Time Period");
    axisX->setRange(01, 12);
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QValueAxis;
    axisY->setTitleText("Concentration (μg/L)");
    axisY->setRange(0, 100);
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Chart view with dark theme
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(500);
    chartView->setBackgroundBrush(QColor("#2F3A4F"));
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