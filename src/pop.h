#ifndef POP_H
#define POP_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QFrame>
#include <QtCharts>
#include <QList>

class Pop : public QWidget
{
    Q_OBJECT

public:
    explicit Pop(QWidget *parent = nullptr);
    QString getSafetyAnalysisText() const;

private slots:
    void onLocationChanged(int index);
    void updateTooltip(const QPointF &point, bool state);

private:
    void loadDataFromFile();
    void initializePollutantInfo();
    void setupCustomTooltips();
    void setupAnalysisSection();
    void addColorRanges();
    void calculateAndDisplaySafetyLevels(const QVector<QPair<double, int>> &averagedCoords);

    // Layout
    QVBoxLayout *mainLayout;
    QComboBox *locationDropdown;

    // Chart elements
    QChartView *chartView;
    QChart *chart;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QGraphicsSimpleTextItem *m_tooltip;
    QList<QGraphicsRectItem *> colorRanges;

    // Analysis section
    QLabel *safetyAnalysisLabel;
    QFrame *analysisContainer;
    QVBoxLayout *analysisLayout;

    // Data structure for location data
    QMap<QString, QVector<QPair<double, int>>> locationData;

    struct PollutantInfo
    {
        QString name;
        QString healthRisks;
        QString safetyLevel;
        QString importance;
    };
    QMap<QString, PollutantInfo> pollutantInfoMap;
};

#endif // POP_H