#include "Compound.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QXYSeries>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QtDebug>
#include <iostream>
#include "data/config.h"

Compound::Compound(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadChartData();
}

void Compound::setupUI()
{
    mainLayout = new QVBoxLayout(this);

    // Initialize and configure filters layout
    QHBoxLayout *filtersLayout = new QHBoxLayout();

    // Setup location filter with predefined items
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All Locations");
    locationFilter->addItem("AIRE AT BEAL BRIDGE");
    locationFilter->addItem("OUSE AT NETHER POPPLETON (SKELTON BANK)");
    locationFilter->addItem("WEST SHAW FARM HAWES 3876");
    locationFilter->addItem("SKELL AT WOOD BRIDGE - RIPON");
    locationFilter->setFixedHeight(30);
    locationFilter->setStyleSheet("QComboBox::item { padding: 5px; }");
    locationFilter->setItemData(0, QFont("Arial", 10, QFont::Bold), Qt::FontRole);
    filtersLayout->addWidget(locationFilter);

    // Setup compound filter with predefined items
    compoundFilter = new QComboBox(this);
    compoundFilter->addItem("All Compounds");
    compoundFilter->addItem("PFOS");
    compoundFilter->addItem("PFHxS-B");
    compoundFilter->addItem("HFPO-DA");
    compoundFilter->addItem("PFBS");
    compoundFilter->setFixedHeight(30);
    compoundFilter->setStyleSheet("QComboBox::item { padding: 5px; }");
    compoundFilter->setItemData(0, QFont("Arial", 10, QFont::Bold), Qt::FontRole);
    filtersLayout->addWidget(compoundFilter);

    filtersLayout->setContentsMargins(0, 0, 0, 0);
    filtersLayout->setSpacing(10);

    mainLayout->addLayout(filtersLayout);

    // Initialize chart view
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Add explanatory label for compliance indicators
    QLabel *explanationLabel = new QLabel("Compliance Indicators use traffic-light colours to show safety levels:\n"
                                          "  -Green if < 0.08 µg/L\n"
                                          "  -Yellow if > 0.08 µg/L and < 0.1 µg/L\n"
                                          "  -Red if > 0.1 µg/L",
                                          this);
    explanationLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    mainLayout->addWidget(explanationLabel);

    // Initialize compound boxes layout
    QHBoxLayout *compoundBoxLayout = new QHBoxLayout();
    pfosBox = new QLabel("PFOS", this);
    pfhxsBBox = new QLabel("PFHxS-B", this);
    hfpoDABBox = new QLabel("HFPO-DA", this);
    pfbsBox = new QLabel("PFBS", this);

    // Add compound boxes to the layout
    compoundBoxes.append(pfosBox);
    compoundBoxes.append(pfhxsBBox);
    compoundBoxes.append(hfpoDABBox);
    compoundBoxes.append(pfbsBox);

    // Set styles for each compound box
    for (QLabel *box : compoundBoxes)
    {
        box->setFixedSize(100, 100);
        box->setAlignment(Qt::AlignCenter);
        box->setStyleSheet("background-color: green; color: white; font-size: 16px; border-radius: 15px;");
        compoundBoxLayout->addWidget(box);
    }

    mainLayout->addLayout(compoundBoxLayout);
    setLayout(mainLayout);

    // Connect filter changes to data loading
    connect(locationFilter, &QComboBox::currentTextChanged, this, &Compound::loadChartData);
    connect(compoundFilter, &QComboBox::currentTextChanged, this, &Compound::loadChartData);
}

void Compound::loadChartData()
{
    try
    {
        // Update model data from CSV file
        model.updateFromFile(Config::UNIVERSAL_FILE_PATH);
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(this, "CSV File Error", error.what());
        return;
    }

    // Retrieve selected location and compound filters
    QString selectedLocation = locationFilter->currentText();
    QString selectedCompound = compoundFilter->currentText();

    // Initialize and configure chart
    QChart *chart = new QChart();
    chart->setTitle("Compound Concentrations Over Time (2024)");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundBrush(Qt::NoBrush);
    chartView->setMinimumHeight(500);
    chartView->setBackgroundBrush(QColor("#2F3A4F"));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelBrush(QBrush(Qt::white));

    // Set custom title font
    QFont titleFont = chart->titleFont();
    titleFont.setBold(true);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(Qt::white));

    QStringList compounds = {"PFOS", "PFHxS-B", "HFPO-DA", "PFBS"};
    QList<QLineSeries *> seriesList;
    QMap<QString, double> compoundValues; // Store compound values for color updates

    // Filter compounds based on selection
    if (selectedCompound != "All Compounds")
    {
        compounds = {selectedCompound};
    }

    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::lowest();

    // Loop through each compound and append data to chart series
    for (const QString &compound : compounds)
    {
        QLineSeries *series = new QLineSeries();
        series->setName(compound);

        // Assign specific colors for each compound
        QPen pen;
        if (compound == "PFOS")
            pen.setColor(QColor("#FF5733"));
        else if (compound == "PFHxS-B")
            pen.setColor(QColor("#33FF57"));
        else if (compound == "HFPO-DA")
            pen.setColor(QColor("#f55ff3"));
        else if (compound == "PFBS")
            pen.setColor(QColor("#FFD700"));
        else
            pen.setColor(QColor("#FFFFFF"));

        pen.setWidth(3);
        series->setPen(pen);

        // Calculate monthly sums for the selected year
        double maxConcentration = 0.0;
        for (int month = 1; month <= 9; ++month)
        {
            QString monthStr = QString::number(month).rightJustified(2, '0');
            QString timeFilter = "2024-" + monthStr;

            double sum = model.sumCompound(compound, selectedLocation, timeFilter);
            if (std::isnan(sum))
            {
                qWarning() << "Invalid result for" << compound << "in month" << timeFilter;
                sum = 0.0;
            }

            series->append(month, sum);
            maxConcentration = std::max(maxConcentration, sum);

            yMin = std::min(yMin, sum);
            yMax = std::max(yMax, sum);
        }

        compoundValues[compound] = maxConcentration;
        seriesList.append(series);
        chart->addSeries(series);
    }

    // Update compound box colors based on concentration values
    for (int i = 0; i < compoundBoxes.size(); ++i)
    {
        QString compoundName = compoundBoxes.at(i)->text();
        if (compoundValues.contains(compoundName))
        {
            double concentration = compoundValues[compoundName];
            if (concentration > 0.1)
            {
                compoundBoxes.at(i)->setStyleSheet("background-color: red; color: white;");
            }
            else if (concentration > 0.08)
            {
                compoundBoxes.at(i)->setStyleSheet("background-color: yellow; color: black;");
            }
            else
            {
                compoundBoxes.at(i)->setStyleSheet("background-color: green; color: white;");
            }
        }
    }

    // Setup X-axis with months
    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->setTitleText("Month");
    axisX->setLabelsBrush(QBrush(Qt::white));
    axisX->setTitleBrush(QBrush(Qt::white));
    for (int i = 1; i <= 12; ++i)
    {
        axisX->append(QString("M%1").arg(i), i);
    }
    chart->addAxis(axisX, Qt::AlignBottom);

    // Setup Y-axis with concentration values
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Concentration in µg/L");
    axisY->setLabelsBrush(QBrush(Qt::white));
    axisY->setTitleBrush(QBrush(Qt::white));

    // Set dynamic Y-axis range
    axisY->setRange(0.0, yMax + (yMax - yMin) * 0.1);
    axisY->setLabelFormat("%.3f");
    axisY->setTickCount(10);

    // Set grid lines for both axes
    QPen gridPen;
    gridPen.setColor(QColor(255, 255, 255, 50));
    gridPen.setWidth(1);

    axisX->setGridLinePen(gridPen);
    axisY->setGridLinePen(gridPen);

    chart->addAxis(axisY, Qt::AlignLeft);

    // Attach axes to all series
    for (QLineSeries *series : seriesList)
    {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    chartView->setChart(chart); // Set the chart to the view
}