#ifndef POLLUTANTOVERVIEW_H
#define POLLUTANTOVERVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QtCharts>
#include <QMessageBox>
#include <QToolTip>
#include <QSet>
#include <QFile>
#include <QTextStream>

class PollutantOverview : public QWidget
{
    Q_OBJECT

public:
    explicit PollutantOverview(QWidget *parent = nullptr);

private slots:
    void loadChartData();
    void showToolTip(const QPointF &point);
    void onPointClicked(QPointF point);

private:
    void setupUI();
    void loadInitialData();
    void setupSeriesTooltips(QLineSeries *series);
    void setupClickEvent(QLineSeries *series);

    QVBoxLayout *mainLayout;
    QComboBox *locationFilter;
    QComboBox *pollutantSearchBar;
    QChartView *chartView;
    QChart *chart;
};

#endif // POLLUTANTOVERVIEW_H