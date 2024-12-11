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
        QComboBox QAbstractItemView {
            background-color: #2F3A4F;
            color: white;
            selection-background-color: #4A5A76;
            selection-color: white;
            border: none;
            outline: none;
            padding: 5px;
        }
        QComboBox QAbstractItemView::item {
            min-height: 25px;
        }
        QComboBox QAbstractItemView::item:hover {
            background-color: #4A5A76;
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

    // Setup analysis section
    setupAnalysisSection();

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

    // Connect signal before loading data
    connect(locationDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Pop::onLocationChanged);

    // Load data
    loadDataFromFile();
}

void Pop::setupAnalysisSection()
{
    // Create container without styling
    analysisContainer = new QFrame(this);

    // Create layout for analysis
    analysisLayout = new QVBoxLayout(analysisContainer);
    analysisLayout->setContentsMargins(0, 15, 0, 0); // Just top margin
    analysisLayout->setSpacing(15);

    // Create label for analysis
    safetyAnalysisLabel = new QLabel(this);
    safetyAnalysisLabel->setWordWrap(true);
    safetyAnalysisLabel->setStyleSheet("QLabel { color: white; font-size: 14px; line-height: 1.6; }");
    analysisLayout->addWidget(safetyAnalysisLabel);

    mainLayout->addWidget(analysisContainer);
    analysisContainer->hide();
}

void Pop::calculateAndDisplaySafetyLevels(const QVector<QPair<double, int>> &averagedCoords)
{
    int totalPoints = averagedCoords.size();
    if (totalPoints == 0)
        return;

    int safeCount = 0;
    int warningCount = 0;
    int dangerCount = 0;

    for (const auto &coord : averagedCoords)
    {
        double value = coord.first;
        if (value >= 0.00001 && value < 0.001)
        {
            safeCount++;
        }
        else if (value >= 0.001 && value < 0.005)
        {
            warningCount++;
        }
        else if (value >= 0.005)
        {
            dangerCount++;
        }
    }

    double safePercent = (safeCount * 100.0) / totalPoints;
    double warningPercent = (warningCount * 100.0) / totalPoints;
    double dangerPercent = (dangerCount * 100.0) / totalPoints;

    QString analysis = QString(
                           "<p style='font-size: 18px; color: #f55ff3; margin: 0 0 15px 0; text-align: center;'>"
                           "<b>📊 Safety Level Analysis</b></p>"

                           "<div style='background-color: rgba(107, 203, 119, 0.1); padding: 10px; border-radius: 6px; margin: 8px 0;'>"
                           "<span style='color: #6bcb77; font-size: 15px;'>✓ Safe Level (0.00001-0.00099 μg/L): "
                           "<b>%1%</b></span></div>"

                           "<div style='background-color: rgba(255, 217, 61, 0.1); padding: 10px; border-radius: 6px; margin: 8px 0;'>"
                           "<span style='color: #ffd93d; font-size: 15px;'>⚠️ Warning Level (0.001-0.00499 μg/L): "
                           "<b>%2%</b></span></div>"

                           "<div style='background-color: rgba(255, 107, 107, 0.1); padding: 10px; border-radius: 6px; margin: 8px 0;'>"
                           "<span style='color: #ff6b6b; font-size: 15px;'>🚨 Danger Level (≥0.005 μg/L): "
                           "<b>%3%</b></span></div>")
                           .arg(safePercent, 0, 'f', 1)
                           .arg(warningPercent, 0, 'f', 1)
                           .arg(dangerPercent, 0, 'f', 1);

    // Enhanced recommendation styling
    QString recommendation;
    if (dangerPercent > 0)
    {
        recommendation = QString(
            "<div style='background-color: rgba(255, 107, 107, 0.1); padding: 15px; border-radius: 8px; margin-top: 10px;'>"
            "<p style='color: #ff6b6b; font-size: 16px; margin: 0;'>"
            "<b>🚨 High Risk Assessment</b></p>"
            "<p style='color: #ff6b6b; margin: 10px 0 0 0;'>"
            "Presence of dangerous pollutant levels detected. Immediate action recommended for water quality improvement.</p>"
            "</div>");
    }
    else if (warningPercent > 0)
    {
        recommendation = QString(
            "<div style='background-color: rgba(255, 217, 61, 0.1); padding: 15px; border-radius: 8px; margin-top: 10px;'>"
            "<p style='color: #ffd93d; font-size: 16px; margin: 0;'>"
            "<b>⚠️ Moderate Risk Assessment</b></p>"
            "<p style='color: #ffd93d; margin: 10px 0 0 0;'>"
            "Pollutants at warning levels detected. Regular monitoring and preventive measures advised.</p>"
            "</div>");
    }
    else
    {
        recommendation = QString(
            "<div style='background-color: rgba(107, 203, 119, 0.1); padding: 15px; border-radius: 8px; margin-top: 10px;'>"
            "<p style='color: #6bcb77; font-size: 16px; margin: 0;'>"
            "<b>✓ Low Risk Assessment</b></p>"
            "<p style='color: #6bcb77; margin: 10px 0 0 0;'>"
            "All measurements within safe levels. Continue regular monitoring to maintain water quality.</p>"
            "</div>");
    }

    safetyAnalysisLabel->setText(analysis + recommendation);
    analysisContainer->show();
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

    // Temporarily block signals
    locationDropdown->blockSignals(true);

    // Clear and initialize dropdown
    locationDropdown->clear();
    locationDropdown->addItem("Select Location");

    // Get sorted list of locations
    QStringList locations = locationData.keys();
    locations.sort();

    // Add all locations to dropdown
    for (const QString &location : locations)
    {
        locationDropdown->addItem(location);
    }

    // Re-enable signals
    locationDropdown->blockSignals(false);

    // Set the first actual location as default if available
    if (locationDropdown->count() > 1)
    {
        locationDropdown->setCurrentIndex(1);
        onLocationChanged(1);
    }
}

void Pop::onLocationChanged(int index)
{
    series->clear();

    // Hide analysis container and clear color ranges if no location selected
    if (index <= 0)
    {
        analysisContainer->hide();
        addColorRanges();
        return;
    }

    QString location = locationDropdown->itemText(index);
    if (locationData.contains(location))
    {
        const auto &originalCoords = locationData[location];

        QMap<int, QPair<double, int>> monthData;

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
                data.first += value;
                data.second += 1;
            }
        }

        QVector<QPair<double, int>> averagedCoords;
        double maxY = 0;
        double minY = std::numeric_limits<double>::max();

        for (auto it = monthData.begin(); it != monthData.end(); ++it)
        {
            int month = it.key();
            double average = it.value().first / it.value().second;
            averagedCoords.append(qMakePair(average, month));
            maxY = qMax(maxY, average);
            minY = qMin(minY, average);
        }

        std::sort(averagedCoords.begin(), averagedCoords.end(),
                  [](const QPair<double, int> &a, const QPair<double, int> &b)
                  {
                      return a.second < b.second;
                  });

        for (const auto &coord : averagedCoords)
        {
            series->append(coord.second, coord.first);
        }

        if (maxY == minY)
        {
            double value = maxY;
            maxY = value * 1.5;
            minY = value * 0.5;
            if (minY < 0)
                minY = 0;
        }

        axisY->setRange(minY, maxY * 1.2);

        if (minMonth == maxMonth)
        {
            axisX->setRange(qMax(1, minMonth - 1), qMin(12, minMonth + 1));
        }
        else
        {
            axisX->setRange(0, 9);
        }

        addColorRanges();
        calculateAndDisplaySafetyLevels(averagedCoords);

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

    // If no location selected, set default blue background
    if (locationDropdown->currentIndex() <= 0)
    {
        chart->setPlotAreaBackgroundVisible(true);
        chart->setPlotAreaBackgroundBrush(QColor("#2F3A4F"));
        return;
    }

    // If location is selected, hide default background
    chart->setPlotAreaBackgroundVisible(false);

    // Get current Y-axis range
    double yMin = axisY->min();
    double yMax = axisY->max();

    // Define threshold values
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

QString Pop::getSafetyAnalysisText() const
{
    return safetyAnalysisLabel->text();
}