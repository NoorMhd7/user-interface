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
#include <QFont>
#include <QtDebug>
#include "data/config.h"

LitterIndicators::LitterIndicators(QWidget *parent)
    : QWidget(parent)
{
    setupUI();       // Set up the user interface
    loadChartData(); // Get some initial chart data
}

void LitterIndicators::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Horizontal layout for filters, you know, to choose location and material
    QHBoxLayout *filterLayout = new QHBoxLayout();

    // Adding location filter (lots of places to pick from!)
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All Locations");
    locationFilter->addItem("BRIDLINGTON NORTH (08000)");
    locationFilter->addItem("CAYTON BAY (07500)");
    locationFilter->addItem("DANES DYKE - FLAMBOROUGH (07950)");
    locationFilter->addItem("FILEY (07600)");
    locationFilter->addItem("FLAMBOROUGH SOUTH LANDING (07900)");

    // Set the first item (All Locations) to bold
    locationFilter->setItemData(0, QFont("Arial", 10, QFont::Bold), Qt::FontRole);
    locationFilter->setStyleSheet("QComboBox QAbstractItemView::item { padding: 10px; }");

    filterLayout->addWidget(locationFilter);

    // Adding material type filter (more choices)
    materialTypeFilter = new QComboBox(this);
    materialTypeFilter->addItem("All Water Body Types");
    materialTypeFilter->addItem("SEA WATER");
    materialTypeFilter->addItem("RIVER / RUNNING SURFACE WATER");

    // Bold for the first item here too
    materialTypeFilter->setItemData(0, QFont("Arial", 10, QFont::Bold), Qt::FontRole);
    materialTypeFilter->setStyleSheet("QComboBox QAbstractItemView::item { padding: 10px; }");

    filterLayout->addWidget(materialTypeFilter);

    // Giving the filters a little more room to breathe
    locationFilter->setFixedHeight(40);     
    materialTypeFilter->setFixedHeight(40); 

    // Put the filters into the main layout
    mainLayout->addLayout(filterLayout);

    // Connect filters to refresh chart data whenever something changes
    connect(locationFilter, &QComboBox::currentIndexChanged, this, &LitterIndicators::loadChartData);
    connect(materialTypeFilter, &QComboBox::currentIndexChanged, this, &LitterIndicators::loadChartData);

    // The chart! Where the magic happens.
    chartView = new QChartView(this); // It's just a chart, no big deal
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Add a description label for the chart, you know, to explain stuff
    descriptionLabel = new QLabel(this);
    descriptionLabel->setWordWrap(true); 
    descriptionLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    descriptionLabel->setStyleSheet(
        "color: white; "
        "font-size: 14px; "
        "font-family: Arial, sans-serif; "
        "font-weight: bold; "
        "background-color: rgba(0, 0, 0, 0.6); "
        "border-radius: 8px; "
        "padding: 15px; "
        "margin: 10px 0px;"
    );

    // Stick the description label in the layout
    mainLayout->addWidget(descriptionLabel);

    setLayout(mainLayout);
}

void LitterIndicators::loadChartData()
{
    // Get data from the model (hopefully it's all good)
    try
    {
        model.updateFromFile(Config::CSV_FILE);
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(this, "CSV File Error", error.what());
        return; // Stop here if the data doesn't load
    }

    QStringList materialTypes = {tr("Plastic"), tr("Animal Faeces"), tr("Sewage Debris"),
                                 tr("Tarry Residues"), tr("Glass"), tr("Metal Parts")};

    QList<double> materialCounts;

    // What are we looking at today? Location and material type
    QString selectedLocation = locationFilter->currentText();
    QString selectedMaterialType = materialTypeFilter->currentText();

    QStringList litterTypes = {"BWP - O.L.", "BWP - A.F.", "SewageDebris",
                               "TarryResidus", "No of dogs", "BWP - A.B."};

    for (const QString &type : litterTypes)
    {
        double count = model.sumResult(type, selectedLocation, selectedMaterialType);
        if (std::isnan(count))
        {
            qWarning() << "Invalid result for" << type;
            count = 0.0; // Fix any weirdness
        }
        materialCounts.append(count);
    }

    // Make a chart set with the data
    QBarSet *barSet = new QBarSet(tr("Material Counts"));
    barSet->setPen(QPen(QColor("#f55ff3"), 2));
    barSet->setColor(QColor("#f55ff3"));
    for (double count : materialCounts)
    {
        *barSet << count;
    }

    QBarSeries *series = new QBarSeries();
    series->append(barSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Litter Types and Their Counts"));
    chart->setTitleBrush(QBrush(Qt::white)); // Make the title pop with white
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Transparent background so the focus is on the chart
    chart->setBackgroundBrush(Qt::NoBrush);
    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundBrush(Qt::NoBrush);
    chart->setPlotAreaBackgroundVisible(false);

    // X-axis with all the materials
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(materialTypes);             // The categories (material types)
    axisX->setLabelsBrush(QBrush(Qt::white)); // Make the labels white
    axisX->setGridLinePen(QPen(QColor(255, 255, 255, 50), 0.5)); // Subtle grid lines
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Y-axis (count of the materials)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(tr("Count"));
    axisY->setTitleBrush(QBrush(Qt::white));  // White title
    axisY->setLabelsBrush(QBrush(Qt::white)); // White labels
    axisY->setGridLinePen(QPen(QColor(255, 255, 255, 50), 0.5)); // Subtle grid lines
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

    // Make the chart pretty and stick it in the chartView
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setBackgroundBrush(Qt::NoBrush);
    chartView->setStyleSheet("background: transparent;");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelBrush(QBrush(Qt::white));

    // Update the description label to reflect the data
    QString descriptionText = tr("Physical Litter Counts in ") + selectedLocation + tr(" and in ") + selectedMaterialType + tr(":\n\n")
                              + tr("Plastic: ") + QString::number(materialCounts[0]) 
                              + "\n" + tr("Animal Faeces: ") + QString::number(materialCounts[1]) 
                              + "\n" + tr("Sewage Debris: ") + QString::number(materialCounts[2]) 
                              + "\n" + tr("Tarry Residues: ") + QString::number(materialCounts[3]) 
                              + "\n" + tr("Glass: ") + QString::number(materialCounts[4]) 
                              + "\n" + tr("Metal Parts: ") + QString::number(materialCounts[5]);

    descriptionLabel->setText(descriptionText);
}
