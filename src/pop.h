#ifndef POP_H
#define POP_H

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QVector>

class QVBoxLayout;
class QComboBox;
class QChartView;
class QChart;
class QLineSeries;
class QValueAxis;
class QGraphicsSimpleTextItem;
class QGraphicsRectItem;

class Pop : public QWidget
{
    Q_OBJECT

public:
    explicit Pop(QWidget *parent = nullptr);

private slots:
    void onLocationChanged(int index);
    void updateTooltip(const QPointF &point, bool state);
    void addColorRanges(); // Add this new function

private:
    void loadDataFromFile();
    void setupCustomTooltips();

    QVBoxLayout *mainLayout;
    QComboBox *locationDropdown;
    QChartView *chartView;
    QChart *chart;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QGraphicsSimpleTextItem *m_tooltip;
    QList<QGraphicsRectItem *> colorRanges; // Add this new member variable

    struct PollutantInfo
    {
        QString name;
        QString healthRisks;
        QString safetyLevel;
        QString importance;
    };

    QMap<QString, PollutantInfo> pollutantInfoMap;
    QMap<QString, QVector<QPair<double, int>>> locationData;
};

#endif // POP_H