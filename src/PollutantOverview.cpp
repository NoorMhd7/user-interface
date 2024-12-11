#include "PollutantOverview.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>

#include <QStringList>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QtDebug>
#include <QMessageBox>
#include "data/config.h"

PollutantOverview::PollutantOverview(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadInitialData();
}

void PollutantOverview::setupUI()
{
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel(tr("Pollutants Time-Series Chart"), this);
    QFont titleFont("Arial", 24, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #f55ff3;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Location Dropdown
    locationFilter = new QComboBox(this);
    locationFilter->setFixedWidth(200);
    locationFilter->setFixedHeight(30);
    locationFilter->setStyleSheet(R"(
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
    mainLayout->addWidget(locationFilter, 0, Qt::AlignLeft);

    // Pollutant Dropdown
    pollutantSearchBar = new QComboBox(this);
    pollutantSearchBar->setFixedWidth(200);
    pollutantSearchBar->setEditable(true);
    pollutantSearchBar->setPlaceholderText("Search for pollutants...");
    pollutantSearchBar->setStyleSheet(R"(
        QComboBox {
            background-color: #4A5A76;
            color: white;
            border: none;
            padding: 5px;
            border-radius: 4px;
            font-size: 16px;
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
    mainLayout->addWidget(pollutantSearchBar, 0, Qt::AlignLeft);

    // Create chart
    chart = new QChart();
    chart->setTitle("Pollutant Concentrations Over Time");
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setBackgroundBrush(QColor("#2F3A4F"));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setLabelBrush(QBrush(Qt::white));

    // Create axes
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;

    axisX->setTitleText(tr("Month"));
    axisX->setRange(1, 12);
    axisX->setLabelsColor(Qt::white);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY->setTitleText(tr("Concentration"));
    axisY->setLabelsColor(Qt::white);
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setGridLineColor(QColor("#4A5A76"));
    chart->addAxis(axisY, Qt::AlignLeft);

    // Chart view
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(500);
    chartView->setBackgroundBrush(QColor("#2F3A4F"));
    mainLayout->addWidget(chartView, 1);

    // Connect signals
    connect(locationFilter, &QComboBox::currentTextChanged, this, &PollutantOverview::loadChartData);
    connect(pollutantSearchBar, &QComboBox::currentTextChanged, this, &PollutantOverview::loadChartData);
}

void PollutantOverview::loadInitialData()
{
    QFile file("../src/data/.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error", "Could not open file");
        return;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine(); // Skip header

    QSet<QString> uniqueLocations;
    QSet<QString> uniquePollutants;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (fields.size() >= 4)
        {
            uniqueLocations.insert(fields[0].trimmed());
            uniquePollutants.insert(fields[2].trimmed());
        }
    }

    // Populate dropdowns
    locationFilter->clear();
    locationFilter->addItem(tr("Select Location"));
    QStringList sortedLocations = uniqueLocations.values();
    std::sort(sortedLocations.begin(), sortedLocations.end());
    locationFilter->addItems(sortedLocations);

    pollutantSearchBar->clear();
    pollutantSearchBar->addItem(tr("Select Pollutant"));
    QStringList sortedPollutants = uniquePollutants.values();
    std::sort(sortedPollutants.begin(), sortedPollutants.end());
    pollutantSearchBar->addItems(sortedPollutants);

    file.close();
}

void PollutantOverview::loadChartData()
{
    QString selectedLocation = locationFilter->currentText();
    QString selectedPollutant = pollutantSearchBar->currentText();

    // Skip if no valid selection
    if (selectedLocation == tr("Select Location") ||
        selectedPollutant == tr("Select Pollutant"))
    {
        return;
    }

    QFile file("../src/data/.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Error", "Could not open file");
        return;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine(); // Skip header

    // Create new series
    QLineSeries *series = new QLineSeries();
    series->setName(selectedPollutant);
    series->setPen(QPen(QColor("#f55ff3"), 2));

    QVector<QPair<int, double>> dataPoints;
    double maxY = 0;

    // Debug output
    qDebug() << "Loading data for:" << selectedLocation << selectedPollutant;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (fields.size() >= 4)
        {
            QString location = fields[0].trimmed();
            QString pollutant = fields[2].trimmed();

            if (location == selectedLocation && pollutant == selectedPollutant)
            {
                bool ok1, ok2;
                int month = fields[1].trimmed().toInt(&ok1);
                double concentration = fields[3].trimmed().toDouble(&ok2);

                if (ok1 && ok2)
                {
                    dataPoints.append(qMakePair(month, concentration));
                    maxY = qMax(maxY, concentration);
                    qDebug() << "Added point:" << month << concentration;
                }
            }
        }
    }

    // Sort by month and add to series
    std::sort(dataPoints.begin(), dataPoints.end());
    for (const auto &point : dataPoints)
    {
        series->append(point.first, point.second);
    }

    // Update chart
    chart->removeAllSeries();
    chart->addSeries(series);

    // Update axes
    chart->axes(Qt::Horizontal).first()->setRange(1, 12);
    chart->axes(Qt::Vertical).first()->setRange(0, maxY * 1.2); // 20% padding

    // Make sure series is attached to both axes
    series->attachAxis(qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first()));
    series->attachAxis(qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first()));

    // Update chart title
    chart->setTitle(QString("%1 - %2 (%3 measurements)")
                        .arg(selectedLocation)
                        .arg(selectedPollutant)
                        .arg(dataPoints.size()));

    file.close();

    // Debug output
    qDebug() << "Total points plotted:" << dataPoints.size();
}

void PollutantOverview::setupSeriesTooltips(QLineSeries *series)
{
    connect(series, &QLineSeries::hovered, this, &PollutantOverview::showToolTip);
}

void PollutantOverview::showToolTip(const QPointF &point)
{
    QString toolTipText = QString("Month: %1\nConcentration: %2")
                              .arg(point.x())
                              .arg(point.y(), 0, 'f', 4);
    QToolTip::showText(QCursor::pos(), toolTipText);
}

void PollutantOverview::setupClickEvent(QLineSeries *series)
{
    connect(series, &QXYSeries::clicked, this, &PollutantOverview::onPointClicked);
}

void PollutantOverview::onPointClicked(QPointF point)
{
    QString details = QString("Month: %1\nConcentration: %2")
                          .arg(point.x())
                          .arg(point.y(), 0, 'f', 4);
    QMessageBox::information(this, "Measurement Details", details);
}