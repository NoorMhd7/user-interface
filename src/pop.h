// pop.h
#ifndef POP_H
#define POP_H

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QDir>
#include "data/config.h"

class QVBoxLayout;
class QComboBox;
class QChartView;
class QChart;
class QLineSeries;
class QValueAxis;
class QGraphicsSimpleTextItem;

class Pop : public QWidget
{
    Q_OBJECT

public:
    explicit Pop(QWidget *parent = nullptr);

private:
    void loadDataFromFile();
    void initializePollutantInfo();
    void setupCustomTooltips();
    void onLocationChanged(int index);
    void updateTooltip(const QPointF &point, bool state);

    // Data structures
    struct PollutantInfo
    {
        QString name;
        QString healthRisks;
        QString safetyLevel;
        QString importance;
    };

    QVBoxLayout *mainLayout;
    QComboBox *locationDropdown;
    QChartView *chartView;
    QChart *chart;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QGraphicsSimpleTextItem *m_tooltip;

    QMap<QString, QVector<QPair<double, int>>> locationData;
    QMap<QString, PollutantInfo> pollutantInfoMap;
};

#endif // POP_H