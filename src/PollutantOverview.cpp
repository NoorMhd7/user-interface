#include "PollutantOverview.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLineSeries>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>
#include <QComboBox>
#include <QPushButton>
#include <QFrame>
#include <QtCharts>

PollutantOverview::PollutantOverview(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Header Section
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *pageTitle = new QLabel("Pollutant Overview", this);
    pageTitle->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(pageTitle);

    // Search Bar
    QLineEdit *searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search pollutants by name...");
    searchBar->setMaximumWidth(550);
    headerLayout->addWidget(searchBar);
    mainLayout->addLayout(headerLayout);

    // List of pollutants for auto-suggestions
    QStringList pollutantSuggestions = fetchPollutantSuggestions();
    QCompleter *completer = new QCompleter(pollutantSuggestions, this);
    searchBar->setCompleter(completer);

    // Filters
    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Filter by:", this);
    filtersLayout->addWidget(filterLabel);

    QComboBox *timeRangeFilter = new QComboBox(this);
    timeRangeFilter->addItem("Last Month");
    timeRangeFilter->addItem("Last Year");
    filtersLayout->addWidget(timeRangeFilter);

    QComboBox *regionFilter = new QComboBox(this);
    regionFilter->addItem("All Regions");
    regionFilter->addItem("Region 1");
    regionFilter->addItem("Region 2");
    filtersLayout->addWidget(regionFilter);

    QString region = regionFilter->currentText();
    QString timeRange = timeRangeFilter->currentText();
    QStringList newSuggestions = fetchPollutantSuggestionsBasedOnFilters(region, timeRange);
    completer->setModel(new QStringListModel(newSuggestions, completer));

    mainLayout->addLayout(filtersLayout);

    // Visualization Section
    QHBoxLayout *visualizationLayout = new QHBoxLayout();

    // Time-Series Chart for Pollutant Levels
    QChartView *timeSeriesChart = new QChartView(this);
    QChart *chart = new QChart();
    chart->setTitle("Time-Series of Pollutant Levels");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Create a dynamic series and connect it to your data update method
    QLineSeries *series = new QLineSeries();
    series->setName("Pollutant Levels");
    chart->addSeries(series);

    // Set up axes with custom labels and ranges
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Time");
    axisX->setTickCount(10);
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Pollutant Level (ug/L)");
    axisY->setLabelFormat("%.3f");
    chart->addAxis(axisY, Qt::AlignLeft);

    // Enable panning and zooming for interactivity
    timeSeriesChart->setChart(chart);
    timeSeriesChart->setRenderHint(QPainter::Antialiasing);
    chart->setAcceptHoverEvents(true);
    visualizationLayout->addWidget(timeSeriesChart);

    // Connect the tooltips and click event handlers
    setupSeriesTooltips(series); // For hover tooltips
    setupClickEvent(series);     // For click pop-up dialog

    mainLayout->addLayout(visualizationLayout);

    // Signal-Slot Connections for Filters
    connect(timeRangeFilter, &QComboBox::currentTextChanged, this, [this, series, timeRangeFilter, regionFilter]()
            { this->updateTimeSeriesData(series, timeRangeFilter->currentText(), regionFilter->currentText()); });

    connect(regionFilter, &QComboBox::currentTextChanged, this, [this, series, timeRangeFilter, regionFilter]()
            { this->updateTimeSeriesData(series, timeRangeFilter->currentText(), regionFilter->currentText()); });

    // Compliance Overview Section
    QVBoxLayout *complianceOverviewSection = new QVBoxLayout();

    // Title for Compliance Overview
    QLabel *complianceTitle = new QLabel("Compliance Overview", this);
    complianceTitle->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px; color: #333;");
    complianceTitle->setAlignment(Qt::AlignCenter);
    complianceOverviewSection->addWidget(complianceTitle);

    // Compliance Cards Layout
    QHBoxLayout *complianceOverviewLayout = new QHBoxLayout();
    QStringList complianceColors = {"green", "#FFC107", "red"};
    QList<QPair<QString, QString>> complianceData = {
        {"Area A", "Safe Levels"},
        {"Area B", "Caution"},
        {"Area C", "Exceeds Safe Levels"}};

    for (int i = 0; i < complianceData.size(); ++i)
    {
        QFrame *card = new QFrame(this);
        card->setFrameShape(QFrame::StyledPanel);
        card->setStyleSheet("border: 1px solid gray; border-radius: 5px; padding: 10px; background-color: #f9f9f9;");

        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        // Compliance Indicator
        QLabel *indicator = new QLabel(this);
        indicator->setFixedSize(20, 20);
        indicator->setStyleSheet(QString("border-radius: 10px; background-color: %1;").arg(complianceColors[i]));

        // Compliance Title
        QLabel *regionLabel = new QLabel(complianceData[i].first, this);
        regionLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");

        // Compliance Summary
        QLabel *complianceLabel = new QLabel(complianceData[i].second, this);
        complianceLabel->setStyleSheet("font-size: 14px; color: #555;");

        // Layout the card
        cardLayout->addWidget(indicator, 0, Qt::AlignCenter);
        cardLayout->addWidget(regionLabel, 0, Qt::AlignCenter);
        cardLayout->addWidget(complianceLabel, 0, Qt::AlignCenter);
        complianceOverviewLayout->addWidget(card);
    }

    // Add the cards layout to the compliance overview section
    complianceOverviewSection->addLayout(complianceOverviewLayout);

    // Add the compliance overview section to the main layout
    mainLayout->addLayout(complianceOverviewSection);
}

QStringList PollutantOverview::fetchPollutantSuggestions()
{
    QStringList suggestions;
    QFile file("Y-2024.csv");

    QTextStream in(&file);
    bool isFirstLine = true;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (isFirstLine)
        {
            isFirstLine = false; // Skip header
            continue;
        }

        QStringList fields = line.split('\t');
        if (fields.size() > 5)
        {
            QString pollutantName = fields[5];
            if (!suggestions.contains(pollutantName))
            {
                suggestions.append(pollutantName);
            }
        }
    }

    file.close();
    return suggestions;
}

QStringList PollutantOverview::fetchPollutantSuggestionsBasedOnFilters(const QString &region, const QString &timeRange)
{
    QStringList suggestions;
    QFile file("data/Y-2024.csv");

    QTextStream in(&file);
    bool isFirstLine = true;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (isFirstLine)
        {
            isFirstLine = false; // Skip header
            continue;
        }

        QStringList fields = line.split('\t');
        if (fields.size() > 5)
        {
            QString pollutantName = fields[5];
            QString pollutantRegion = fields[3];
            QString pollutantDateTime = fields[4];

            // Parse and compare dates for filtering (timeRange)
            if ((region == "All Regions" || pollutantRegion == region) &&
                (timeRange == "All Time" || isWithinTimeRange(pollutantDateTime, timeRange)))
            {
                if (!suggestions.contains(pollutantName))
                {
                    suggestions.append(pollutantName);
                }
            }
        }
    }

    file.close();
    return suggestions;
}

bool PollutantOverview::isWithinTimeRange(const QString &dateTimeStr, const QString &timeRange)
{
    QDateTime dateTime = QDateTime::fromString(dateTimeStr, Qt::ISODate);

    if (timeRange == "Last Month")
    {
        QDateTime oneMonthAgo = QDateTime::currentDateTime().addMonths(-1);
        return dateTime >= oneMonthAgo;
    }
    else if (timeRange == "Last Year")
    {
        QDateTime oneYearAgo = QDateTime::currentDateTime().addYears(-1);
        return dateTime >= oneYearAgo;
    }

    return true;
}

void PollutantOverview::updateTimeSeriesData(QLineSeries *series, const QString &timeRange, const QString &region)
{
    // Clear old data
    series->clear();

    QFile file("dataY-2024.csv");

    QTextStream in(&file);
    bool isFirstLine = true;

    QVector<QPointF> dataPoints;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (isFirstLine)
        {
            isFirstLine = false; // Skip header
            continue;
        }

        QStringList fields = line.split('\t');
        if (fields.size() > 9)
        {
            QString pollutantRegion = fields[3];
            QString pollutantDateTime = fields[4];
            double pollutantLevel = fields[9].toDouble();

            // Apply filters
            if ((region == "All Regions" || pollutantRegion == region) &&
                (timeRange == "All Time" || isWithinTimeRange(pollutantDateTime, timeRange)))
            {
                QDateTime dateTime = QDateTime::fromString(pollutantDateTime, Qt::ISODate);
                dataPoints.append(QPointF(dateTime.toMSecsSinceEpoch(), pollutantLevel));
            }
        }
    }

    file.close();

    // Add data points to the series
    for (const QPointF &point : dataPoints)
    {
        series->append(point);
    }

    // Update the chart axes dynamically
    auto chart = series->chart();
    if (chart)
    {
        // Remove old axes
        QList<QAbstractAxis *> axes = chart->axes();
        for (auto axis : axes)
        {
            chart->removeAxis(axis);
            delete axis;
        }

        // Create new axes
        auto xAxis = new QValueAxis();
        auto yAxis = new QValueAxis();

        if (!dataPoints.isEmpty())
        {
            xAxis->setRange(dataPoints.first().x(), dataPoints.last().x());
            double maxY = std::max_element(dataPoints.begin(), dataPoints.end(),
                                           [](const QPointF &a, const QPointF &b)
                                           {
                                               return a.y() < b.y();
                                           })
                              ->y();
            yAxis->setRange(0, maxY * 1.1);
        }

        chart->addAxis(xAxis, Qt::AlignBottom);
        chart->addAxis(yAxis, Qt::AlignLeft);

        series->attachAxis(xAxis);
        series->attachAxis(yAxis);
    }
}

// Setup tooltips for hovering over series data points
void PollutantOverview::setupSeriesTooltips(QLineSeries *series)
{
    connect(series, &QLineSeries::hovered, this, &PollutantOverview::showToolTip);
}

// Display the tooltip when hovering over a data point
void PollutantOverview::showToolTip(const QPointF &point)
{
    // Example Tooltip Content (you can adjust this as needed):
    QString toolTipText = QString("Pollutant Level: %1 µg/L\nTime: %2\nRisk: Moderate\nCompliance: Amber\nSafety Threshold: 10 µg/L")
                              .arg(point.y())
                              .arg(point.x());

    // Display the tooltip at the current mouse cursor position
    QToolTip::showText(QCursor::pos(), toolTipText);
}

// Get compliance status based on pollutant level
QString PollutantOverview::getComplianceStatus(double pollutantLevel)
{
    if (pollutantLevel < 0.5)
    {
        return "Compliant";
    }
    else if (pollutantLevel < 1.0)
    {
        return "Warning";
    }
    else
    {
        return "Non-Compliant";
    }
}

// Get safety threshold based on pollutant level
QString PollutantOverview::getSafetyThreshold(double pollutantLevel)
{
    if (pollutantLevel < 0.5)
    {
        return "Safe";
    }
    else if (pollutantLevel < 1.0)
    {
        return "Caution";
    }
    else
    {
        return "Dangerous";
    }
}

// Setup click event to show detailed pop-up
void PollutantOverview::setupClickEvent(QLineSeries *series)
{
    connect(series, &QXYSeries::clicked, this, &PollutantOverview::showToolTip);
}

// Show a pop-up dialog with detailed information when a data point is clicked
void PollutantOverview::onPointClicked(QPointF point)
{
    QString details = QString("Pollutant Level: %1\nCompliance Status: %2\nSafety Threshold: %3")
                          .arg(point.y())
                          .arg(getComplianceStatus(point.y())) // Get compliance based on pollutant level
                          .arg(getSafetyThreshold(point.y())); // Get safety threshold

    // Display the details in a message box
    QMessageBox::information(this, "Pollutant Data", details);
}
