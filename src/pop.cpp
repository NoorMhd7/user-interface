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
    QLabel *titleLabel = new QLabel(tr("Organic Pollutants Line Chart"), this);
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

        QComboBox QAbstractItemView {
            background-color: #2F3A4F;
    }
    )");

    mainLayout->addWidget(locationDropdown, 0, Qt::AlignLeft);

    // Create chart and axes
    chart = new QChart();
    chart->setTitle("Pollutant Concentrations Trends Over Time");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setBackgroundBrush(QColor("#2F3A4F"));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelBrush(QBrush(Qt::white));

    // Initialize axes
    axisX = new QValueAxis;
    axisY = new QValueAxis;

    // Configure axes
    axisX->setTitleText(tr("Month"));
    axisX->setRange(0, 9);
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY->setTitleText(tr("Concentration (μg/L)"));
    axisY->setRange(0, 1);
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisY, Qt::AlignLeft);

    // Add series
    series = new QLineSeries();
    series->setName(tr("POP and PCB Pollutants"));
    series->setPen(QPen(QColor("#f55ff3"), 2));
    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    // Chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(500);
    chartView->setBackgroundBrush(QColor("#2F3A4F"));
    mainLayout->addWidget(chartView, 1);

    // Create a gradient for the plot area (grid only)
    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart(QPointF(0, 0));
    plotAreaGradient.setFinalStop(QPointF(0, 1));

    chart->setPlotAreaBackgroundVisible(false);
    addColorRanges();
    connect(chart, &QChart::plotAreaChanged, this, &Pop::addColorRanges);

    // Customize grid lines
    axisY->setGridLineVisible(true);
    axisY->setMinorGridLineVisible(true);
    axisY->setMinorTickCount(4);
    axisY->setGridLineColor(QColor(74, 90, 118, 100));
    axisY->setMinorGridLineColor(QColor(74, 90, 118, 50));

    setupCustomTooltips();

    connect(locationDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Pop::onLocationChanged);
    // Load data and connect signals
    loadDataFromFile();
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
                                  "Value: %3 μg/L\n")
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

    // Temporarily disconnect the signal to prevent multiple updates
    locationDropdown->blockSignals(true);

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

    // Re-enable signals
    locationDropdown->blockSignals(false);

    // Set the first actual location (index 1) as default if there are locations
    if (locationDropdown->count() > 1)
    {
        locationDropdown->setCurrentIndex(1);
        // Explicitly call onLocationChanged since we blocked signals
        onLocationChanged(1);
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
        int minMonth = 9;
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
            axisX->setRange(0, 9);
        }
        addColorRanges();

        // Update chart title to show the actual coordinate count
        chart->setTitle(QString("Average POP trends over time (%1 measurements)").arg(originalCoords.size()));
    }
}

void Pop::addColorRanges()
{
    // Clear existing ranges
    for (auto item : colorRanges)
    {
        chart->scene()->removeItem(item);
        delete item;
    }
    colorRanges.clear();

    // Get current Y-axis range
    double yMin = axisY->min();
    double yMax = axisY->max();

    // Only draw ranges that are within the visible range
    // Define our threshold values
    const double greenMin = 0.00001;
    const double greenMax = 0.0009999;
    const double yellowMin = 0.001;
    const double yellowMax = 0.00499999;
    const double redMin = 0.005;

    // Get plot area
    QRectF plotArea = chart->plotArea();

    // Colors with transparency
    const QColor greenColor(107, 203, 119, 40);
    const QColor yellowColor(255, 217, 61, 40);
    const QColor redColor(255, 107, 107, 40);

    // Helper function to map value to pixel position
    auto mapToPixel = [&](double value)
    {
        return plotArea.bottom() - ((value - yMin) / (yMax - yMin) * plotArea.height());
    };

    // Only draw green range if it's in view
    if (greenMin < yMax && greenMax > yMin)
    {
        double top = mapToPixel(std::min(greenMax, yMax));
        double bottom = mapToPixel(std::max(greenMin, yMin));
        QGraphicsRectItem *greenRange = new QGraphicsRectItem();
        greenRange->setRect(plotArea.left(), top, plotArea.width(), bottom - top);
        greenRange->setBrush(greenColor);
        greenRange->setPen(Qt::NoPen);
        chart->scene()->addItem(greenRange);
        colorRanges << greenRange;
    }

    // Only draw yellow range if it's in view
    if (yellowMin < yMax && yellowMax > yMin)
    {
        double top = mapToPixel(std::min(yellowMax, yMax));
        double bottom = mapToPixel(std::max(yellowMin, yMin));
        QGraphicsRectItem *yellowRange = new QGraphicsRectItem();
        yellowRange->setRect(plotArea.left(), top, plotArea.width(), bottom - top);
        yellowRange->setBrush(yellowColor);
        yellowRange->setPen(Qt::NoPen);
        chart->scene()->addItem(yellowRange);
        colorRanges << yellowRange;
    }

    // Only draw red range if it's in view
    if (redMin < yMax)
    {
        double top = mapToPixel(yMax);
        double bottom = mapToPixel(std::max(redMin, yMin));
        QGraphicsRectItem *redRange = new QGraphicsRectItem();
        redRange->setRect(plotArea.left(), top, plotArea.width(), bottom - top);
        redRange->setBrush(redColor);
        redRange->setPen(Qt::NoPen);
        chart->scene()->addItem(redRange);
        colorRanges << redRange;
    }
}