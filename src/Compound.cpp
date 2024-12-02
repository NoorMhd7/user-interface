#include "Compound.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QXYSeries>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>

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
    locationFilter->addItem("Location A");
    locationFilter->addItem("Location B");
    mainLayout->addWidget(locationFilter);

    // Compound filter
    compoundFilter = new QComboBox(this);
    compoundFilter->addItem("All Compounds");
    compoundFilter->addItem("Compound A");
    compoundFilter->addItem("Compound B");
    compoundFilter->addItem("Compound C");
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
    // Create the chart
    QChart *chart = new QChart();
    chart->setTitle("Compound Concentrations Over Time");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Series A
    QLineSeries *seriesA = new QLineSeries();
    seriesA->setName("Compound A");
    seriesA->append(1, 5);
    seriesA->append(2, 10);
    seriesA->append(3, 8);
    connect(seriesA, &QXYSeries::hovered, this, [chart](const QPointF &point, bool state) {
        static QGraphicsSimpleTextItem *tooltip = nullptr;
        if (state) {
            if (!tooltip) {
                tooltip = new QGraphicsSimpleTextItem(chart);
            }
            tooltip->setText(QString("Compound A: (%1, %2)").arg(point.x()).arg(point.y()));
            tooltip->setPos(chart->mapToPosition(point));
            tooltip->show();
        } else if (tooltip) {
            tooltip->hide();
        }
    });
    chart->addSeries(seriesA);

    // Series B
    QLineSeries *seriesB = new QLineSeries();
    seriesB->setName("Compound B");
    seriesB->append(1, 3);
    seriesB->append(2, 6);
    seriesB->append(3, 12);
    connect(seriesB, &QXYSeries::hovered, this, [chart](const QPointF &point, bool state) {
        static QGraphicsSimpleTextItem *tooltip = nullptr;
        if (state) {
            if (!tooltip) {
                tooltip = new QGraphicsSimpleTextItem(chart);
            }
            tooltip->setText(QString("Compound B: (%1, %2)").arg(point.x()).arg(point.y()));
            tooltip->setPos(chart->mapToPosition(point));
            tooltip->show();
        } else if (tooltip) {
            tooltip->hide();
        }
    });
    chart->addSeries(seriesB);

    // Axes
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Time");
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesA->attachAxis(axisX);
    seriesB->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Concentration");
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    seriesA->attachAxis(axisY);
    seriesB->attachAxis(axisY);

    // Assign the chart to the view
    chartView->setChart(chart);
}
