#include "pop.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QtCharts>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QCoreApplication>
#include <algorithm>
#include "data/config.h"

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
    locationDropdown->setStyleSheet(R"(
        QComboBox {
            background-color: #4A5A76;
            color: white;
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

    mainLayout->addWidget(locationDropdown, 0, Qt::AlignLeft);

    // Create chart with dark theme
    chart = new QChart();
    chart->setTitle("Pollutant Concentrations Trends Over Time");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setBackgroundBrush(QColor("#2F3A4F"));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelBrush(QBrush(Qt::white));

    // Create series
    series = new QLineSeries();
    series->setName("POP and PCB Pollutants");
    series->setPen(QPen(QColor("#f55ff3"), 2));
    chart->addSeries(series);

    // Customize axes with white text
    axisX = new QValueAxis;
    axisX->setTitleText("Month");
    axisX->setRange(0, 12);
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis;
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

    // Initialize tooltip and pollutant info
    initializePollutantInfo();
    setupCustomTooltips();

    // Load data and connect signals
    loadDataFromFile();
    connect(locationDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Pop::onLocationChanged);
}

void Pop::initializePollutantInfo()
{
    PollutantInfo pcbInfo;
    pcbInfo.name = "PCB";
    pcbInfo.healthRisks = "Can affect immune system, nervous system, and endocrine system";
    pcbInfo.safetyLevel = "Safe level: < 0.0005 μg/L";
    pcbInfo.importance = "Regular monitoring required for ecosystem health";

    PollutantInfo pbdeInfo;
    pbdeInfo.name = "PBDE";
    pbdeInfo.healthRisks = "May affect thyroid hormone levels and neurodevelopment";
    pbdeInfo.safetyLevel = "Safe level: < 0.0001 μg/L";
    pbdeInfo.importance = "Bioaccumulates in aquatic life";

    pollutantInfoMap["PCB"] = pcbInfo;
    pollutantInfoMap["PBDE"] = pbdeInfo;
}

void Pop::setupCustomTooltips()
{
    m_tooltip = new QGraphicsSimpleTextItem(chart);
    m_tooltip->setZValue(11);
    m_tooltip->hide();

    QFont tooltipFont("Arial", 10);
    m_tooltip->setFont(tooltipFont);
    m_tooltip->setBrush(Qt::white);

    connect(series, &QLineSeries::hovered, this, &Pop::updateTooltip);
}

void Pop::updateTooltip(const QPointF &point, bool state)
{
    if (state)
    {
        QString location = locationDropdown->currentText();
        QString tooltipText = QString(
                                  "Location: %1\n"
                                  "Month: %2\n"
                                  "Value: %3 μg/L\n"
                                  "Safety Level: < 0.0005 μg/L\n"
                                  "Health Risks: Persistent organic pollutants\n"
                                  "can accumulate in the food chain")
                                  .arg(location)
                                  .arg(int(point.x()))
                                  .arg(point.y(), 0, 'f', 6);

        QPointF mappedPoint = chart->mapToPosition(point);
        m_tooltip->setText(tooltipText);
        m_tooltip->setPos(mappedPoint + QPoint(10, -50));
        m_tooltip->show();
    }
    else
    {
        m_tooltip->hide();
    }
}

void Pop::loadDataFromFile()
{
    // Get processed data from Config
    locationData = Config::getProcessedData();

    // Clear and initialize dropdown
    locationDropdown->clear();
    locationDropdown->addItem("Select Location");

    // Get sorted list of locations
    QStringList locations = locationData.keys();
    locations.sort(); // Sort alphabetically

    // Add all locations to dropdown
    for (const QString &location : locations)
    {
        locationDropdown->addItem(location);
    }
}

void Pop::onLocationChanged(int index)
{
    series->clear();

    if (index <= 0)
        return;

    QString location = locationDropdown->itemText(index);
    if (locationData.contains(location))
    {
        // Get coordinates for this location
        const auto &originalCoords = locationData[location];

        // Create a map to store sum and count for each month
        QMap<int, QPair<double, int>> monthData; // month -> (sum, count)

        // Calculate sum and count for each month and track min/max months
        int minMonth = 12;
        int maxMonth = 1;
        for (const auto &coord : originalCoords)
        {
            int month = coord.second;
            double value = coord.first;

            minMonth = qMin(minMonth, month);
            maxMonth = qMax(maxMonth, month);

            if (!monthData.contains(month))
            {
                monthData[month] = qMakePair(value, 1);
            }
            else
            {
                auto &data = monthData[month];
                data.first += value; // Add to sum
                data.second += 1;    // Increment count
            }
        }

        // Create averaged coordinates
        QVector<QPair<double, int>> averagedCoords;
        double maxY = 0;
        double minY = std::numeric_limits<double>::max();

        // Calculate averages and store in new vector
        for (auto it = monthData.begin(); it != monthData.end(); ++it)
        {
            int month = it.key();
            double average = it.value().first / it.value().second;
            averagedCoords.append(qMakePair(average, month));
            maxY = qMax(maxY, average);
            minY = qMin(minY, average);
        }

        // Sort by month
        std::sort(averagedCoords.begin(), averagedCoords.end(),
                  [](const QPair<double, int> &a, const QPair<double, int> &b)
                  {
                      return a.second < b.second;
                  });

        // Plot the averaged and sorted points
        for (const auto &coord : averagedCoords)
        {
            series->append(coord.second, coord.first);
        }

        // Handle axis ranges
        if (maxY == minY)
        {
            // If all values are the same, create a range around the value
            double value = maxY;
            maxY = value * 1.5; // 50% above
            minY = value * 0.5; // 50% below
            if (minY < 0)
                minY = 0;
        }

        // Set axis ranges with padding
        axisY->setRange(minY, maxY * 1.2);

        // If all points are in the same month, extend X axis range
        if (minMonth == maxMonth)
        {
            axisX->setRange(qMax(1, minMonth - 1), qMin(12, minMonth + 1));
        }
        else
        {
            axisX->setRange(0, 12);
        }

        // Update chart title to show the actual coordinate count
        chart->setTitle(QString("Average POP trends over time (%1 measurements)").arg(originalCoords.size()));
    }
}