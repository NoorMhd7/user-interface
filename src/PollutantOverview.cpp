#include "PollutantOverview.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QXYSeries>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QtDebug>
#include <iostream>
#include <QRegularExpression>
#include "data/config.h"

PollutantOverview::PollutantOverview(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadChartData();
}

void PollutantOverview::setupUI()
{
    mainLayout = new QVBoxLayout(this);

    // Initialize and configure search layout
    QHBoxLayout *searchLayout = new QHBoxLayout();

    // Setup search input
    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Search Litter Type (e.g., 1,1,2-Trichloroethane or Chloroform)");
    searchInput->setFixedHeight(30);
    searchLayout->addWidget(searchInput);

    // Setup search button
    searchButton = new QPushButton("Search", this);
    searchButton->setFixedHeight(30);
    searchLayout->addWidget(searchButton);

    mainLayout->addLayout(searchLayout);

    // Initialize chart view
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Initialize litter boxes layout
    litterBoxLayout = new QHBoxLayout();

    // Create a larger compliance indicator rectangle
    QLabel *complianceIndicator = new QLabel("Compliance Indicator", this);
    complianceIndicator->setObjectName("complianceIndicator");
    complianceIndicator->setFixedHeight(150); // Larger height
    complianceIndicator->setAlignment(Qt::AlignCenter);

    // Set up tooltip with detailed explanation
    complianceIndicator->setToolTip(
        "Compliance Indicators use traffic-light colours to show safety levels:\n"
        "  - Green indicates concentration < 0.08 µg/L (Safe)\n"
        "  - Yellow indicates concentration between 0.08 and 0.1 µg/L (Caution)\n"
        "  - Red indicates concentration > 0.1 µg/L (Unsafe)");

    // Enable tooltips
    complianceIndicator->setMouseTracking(true);

    // Default style
    complianceIndicator->setStyleSheet(
        "background-color: green; "
        "color: white; "
        "font-size: 24px; " // Larger font
        "border-radius: 15px; "
        "padding: 20px;");

    mainLayout->addWidget(complianceIndicator);
    setLayout(mainLayout);

    // Connect search button click to data loading
    connect(searchButton, &QPushButton::clicked, this, &PollutantOverview::loadChartData);
    connect(searchInput, &QLineEdit::returnPressed, this, &PollutantOverview::loadChartData);
}

void PollutantOverview::loadChartData()
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

    // Retrieve searched litter type
    QString searchedLitter = searchInput->text();

    // Initialize and configure chart
    QChart *chart = new QChart();
    chart->setTitle(tr("Concentrations Over Time (2024)"));
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

    QList<QLineSeries *> seriesList;
    QMap<QString, double> litterValues;

    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::lowest();

    // Create series only for the searched litter
    QLineSeries *series = new QLineSeries();
    series->setName(searchedLitter);

    // Assign color based on litter type
    QPen pen;
    pen.setColor(QColor("#f55ff3"));
    series->setPen(pen);

    // Calculate monthly sums for the selected year and litter
    double maxConcentration = 0.0;
    for (int month = 1; month <= 9; ++month)
    {
        QString monthStr = QString::number(month).rightJustified(2, '0');
        QString timeFilter = "2024-" + monthStr;

        double sum = model.sumLitter(searchedLitter, timeFilter);
        if (std::isnan(sum))
        {
            qWarning() << "Invalid result for" << searchedLitter << "in month" << timeFilter;
            sum = 0.0;
        }

        series->append(month, sum);
        maxConcentration = std::max(maxConcentration, sum);

        yMin = std::min(yMin, sum);
        yMax = std::max(yMax, sum);
    }

    litterValues[searchedLitter] = maxConcentration;
    seriesList.append(series);
    chart->addSeries(series);

    // Find the compliance indicator label
    QLabel *complianceIndicator = findChild<QLabel *>("complianceIndicator");
    if (complianceIndicator)
    {
        // Update compliance indicator color based on max concentration
        if (maxConcentration > 0.1)
        {
            complianceIndicator->setStyleSheet(
                "background-color: red; "
                "color: white; "
                "font-size: 24px; "
                "border-radius: 15px; "
                "padding: 20px;");
        }
        else if (maxConcentration > 0.08)
        {
            complianceIndicator->setStyleSheet(
                "background-color: yellow; "
                "color: black; "
                "font-size: 24px; "
                "border-radius: 15px; "
                "padding: 20px;");
        }
        else
        {
            complianceIndicator->setStyleSheet(
                "background-color: green; "
                "color: white; "
                "font-size: 24px; "
                "border-radius: 15px; "
                "padding: 20px;");
        }

        currentConcentration = maxConcentration;

        // Update the compliance indicator text
        if (complianceIndicator)
        {
            complianceIndicator->setText(QString("Concentration: %1 µg/L").arg(maxConcentration, 0, 'f', 3));
        }

        // Update the text to show the current concentration
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
    axisY->setTitleText(tr("Concentration in µg/L"));
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

    // Attach axes to the series
    for (QLineSeries *s : seriesList)
    {
        s->attachAxis(axisX);
        s->attachAxis(axisY);
    }

    chartView->setChart(chart); // Set the chart to the view
}