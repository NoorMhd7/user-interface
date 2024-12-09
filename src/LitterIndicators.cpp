#include "LitterIndicators.h"
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QLabel>
#include <QValueAxis>
#include <QMessageBox>
#include <QString>

LitterIndicators::LitterIndicators(QWidget *parent)
    : QWidget(parent)
{
    setupUI();       // Initialize the UI components
    loadChartData(); // Load initial chart data
}

void LitterIndicators::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Filters and refresh button
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All Locations");
    // Add more locations here
    locationFilter->addItem("BRIDLINGTON NORTH (08000)");
    locationFilter->addItem("CAYTON BAY (07500)");
    locationFilter->addItem("DANES DYKE - FLAMBOROUGH (07950)");
    locationFilter->addItem("FILEY (07600)");
    locationFilter->addItem("FLAMBOROUGH SOUTH LANDING (07900)");
    layout->addWidget(locationFilter);

    materialTypeFilter = new QComboBox(this);
    materialTypeFilter->addItem("All Water Body Types");
    // Add more material types here
    materialTypeFilter->addItem("SEA WATER");
    materialTypeFilter->addItem("RIVER / RUNNING SURFACE WATER");
    layout->addWidget(materialTypeFilter);

    refreshButton = new QPushButton("Refresh", this);
    layout->addWidget(refreshButton);

    connect(refreshButton, &QPushButton::clicked, this, &LitterIndicators::loadChartData);

    // Chart view
    chartView = new QChartView(this); // Initialize chartView
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);

    complianceOverview = new QLabel(this);
    complianceOverview->setText("Compliance Overview: All monitored locations comply with standards.");
    complianceOverview->setStyleSheet("font-weight: bold; color: green; margin-top: 10px;");
    layout->addWidget(complianceOverview);

    setLayout(layout);
}

void LitterIndicators::loadChartData()
{
    // Load data from model
    try
    {
        model.updateFromFile("../src/data/Y-2024.csv");
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(this, "CSV File Error", error.what());
        return;
    }

    QStringList materialTypes = {"Plastic", "Animal Faeces", "Sewage Debris",
                                 "Tarry Residues", "Glass", "Metal Parts"};

    QList<double> materialCounts;

    // Get selected location and material type from filters
    QString selectedLocation = locationFilter->currentText();
    QString selectedMaterialType = materialTypeFilter->currentText();

    // List of data types corresponding to material types
    QStringList litterTypes = {"BWP - O.L.", "BWP - A.F.", "SewageDebris",
                               "TarryResidus", "No of dogs", "BWP - A.B."};

    for (const QString &type : litterTypes)
    {
        // Filter by location and material type
        double count = model.sumResult(type, selectedLocation, selectedMaterialType);
        // if (selectedMaterialType != "All Water Body Types" && selectedMaterialType != type) {
        //     continue;  // Skip this type if it doesn't match the selected material type
        // }

        if (std::isnan(count))
        {
            qWarning() << "Invalid result for" << type;
            count = 0.0;
        }
        materialCounts.append(count);
    }

    // Create chart
    QBarSet *barSet = new QBarSet("Material Counts");
    for (double count : materialCounts)
    {
        *barSet << count;
    }

    QBarSeries *series = new QBarSeries();
    series->append(barSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Litter Types and Their Counts");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set X-axis with user-friendly names
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(materialTypes); // Use material types for X-axis
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Count");
    if (!materialCounts.isEmpty() && !std::all_of(materialCounts.begin(), materialCounts.end(), [](double c)
                                                  { return c == 0; }))
    {
        axisY->setRange(0, *std::max_element(materialCounts.begin(), materialCounts.end()) + 5);
    }
    else
    {
        axisY->setRange(0, 5);
    }
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView->setChart(chart); // Set the chart to chartView
}
