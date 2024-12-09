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

Compound::Compound(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadChartData();
}

void Compound::setupUI()
{
    mainLayout = new QVBoxLayout(this);

    // Location filter
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All Locations");
    locationFilter->addItem("AIRE AT BEAL BRIDGE");                     // Placeholder
    locationFilter->addItem("OUSE AT NETHER POPPLETON (SKELTON BANK)"); // Placeholder
    locationFilter->addItem("WEST SHAW FARM HAWES 3876");
    locationFilter->addItem("SKELL AT WOOD BRIDGE - RIPON");

    mainLayout->addWidget(locationFilter);

    // Compound filter
    compoundFilter = new QComboBox(this);
    compoundFilter->addItem("All Compounds");
    compoundFilter->addItem("PFOS");
    compoundFilter->addItem("PFHxS-B");
    compoundFilter->addItem("HFPO-DA");
    compoundFilter->addItem("PFBS");
    mainLayout->addWidget(compoundFilter);

    // Refresh button
    refreshButton = new QPushButton("Refresh", this);
    mainLayout->addWidget(refreshButton);
    connect(refreshButton, &QPushButton::clicked, this, &Compound::loadChartData);

    // Compliance status
    complianceStatus = new QLabel("Compliance Status: Unknown", this);
    complianceStatus->setStyleSheet("color: grey; font-weight: bold;");
    mainLayout->addWidget(complianceStatus);

    // Chart view
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    setLayout(mainLayout);
}

void Compound::loadChartData()
{
    try
    {
        model.updateFromFile("../src/data/Y-2024.csv");
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(this, "CSV File Error", error.what());
        return;
    }
    // Retrieve selected location and compound
    QString selectedLocation = locationFilter->currentText();
    QString selectedCompound = compoundFilter->currentText();

    // Create the chart
    QChart *chart = new QChart();
    chart->setTitle("Compound Concentrations Over Time (2024)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList compounds = {"PFOS", "PFHxS-B", "HFPO-DA", "PFBS"};
    QList<QLineSeries *> seriesList;

    // Filter compounds (if specific compound selected)
    if (selectedCompound != "All Compounds")
    {
        compounds = {selectedCompound};
    }

    double yMin = std::numeric_limits<double>::max();    // Start with max value to find min
    double yMax = std::numeric_limits<double>::lowest(); // Start with lowest value to find max

    for (const QString &compound : compounds)
    {
        QLineSeries *series = new QLineSeries();
        series->setName(compound);

        // Calculate monthly sums for 2024
        for (int month = 1; month <= 9; ++month)
        {
            QString monthStr = QString::number(month).rightJustified(2, '0'); // Format as 01, 02, etc.
            QString timeFilter = "2024-" + monthStr;

            double sum = model.sumCompound(compound, selectedLocation, timeFilter);
            if (std::isnan(sum))
            {
                qWarning() << "Invalid result for" << compound << "in month" << timeFilter;
                sum = 0.0;
            }

            series->append(month, sum);

            // Track min/max values for Y-axis range
            yMin = std::min(yMin, sum);
            yMax = std::max(yMax, sum);
        }

        // Add tooltips for hover events
        connect(series, &QXYSeries::hovered, this, [chart, compound](const QPointF &point, bool state)
                {
            static QGraphicsSimpleTextItem *tooltip = nullptr;
            if (state) {
                if (!tooltip) {
                    tooltip = new QGraphicsSimpleTextItem(chart);
                }
                tooltip->setText(QString("%1: (%2, %3)").arg(compound).arg(point.x()).arg(point.y()));
                tooltip->setPos(chart->mapToPosition(point));
                tooltip->show();
            } else if (tooltip) {
                tooltip->hide();
            } });

        seriesList.append(series);
        chart->addSeries(series);
    }

    // Axes
    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->setTitleText("Month");
    for (int i = 1; i <= 12; ++i)
    {
        axisX->append(QString("M%1").arg(i), i);
    }
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Concentration in µg/L");

    // Set dynamic range for Y-axis
    // Ensure 0 is visible on the Y-axis and avoid negative values
    axisY->setRange(0.0, yMax + (yMax - yMin) * 0.1); // Force the minimum Y value to 0 and add some padding to the max

    axisY->setLabelFormat("%.3f"); // Increase precision for small numbers
    axisY->setTickCount(10);       // Set a more appropriate tick count

    chart->addAxis(axisY, Qt::AlignLeft);

    // Attach axes to all series
    for (QLineSeries *series : seriesList)
    {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    // Update compliance status (example logic)
    complianceStatus->setText("Compliance Status: All measurements within expected range."); // Placeholder

    chartView->setChart(chart); // Set the chart to the view
}
