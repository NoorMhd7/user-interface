#ifndef PollutantOverview_H
#define PollutantOverview_H

#include <QWidget>
#include <QtCharts>
#include <QString>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QMap>
#include <QPair>
#include <QLineSeries>
#include <QLineEdit>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>

class PollutantOverview : public QWidget
{
    Q_OBJECT

public:
    explicit PollutantOverview(QWidget *parent = nullptr);

    // Fetch pollutant suggestions for the search bar (based on CSV data)
    QStringList fetchPollutantSuggestions();

    // Fetch pollutant suggestions based on the region and time range filters
    QStringList fetchPollutantSuggestionsBasedOnFilters(const QString &region, const QString &timeRange);

    // Update the time-series data based on the selected filters
    void updateTimeSeriesData(QLineSeries *series, const QString &timeRange, const QString &region);

private:
    // Member Variables
    QLineEdit *searchBar;
    QCompleter *completer;
    QStringList pollutantSuggestions;
    QComboBox *regionFilter;
    QComboBox *timeRangeFilter;

    // Private Methods
    void setupCompleter();

    bool isWithinTimeRange(const QString &dateTimeStr, const QString &timeRange);

    // Methods for handling tooltips and click events
    void setupSeriesTooltips(QLineSeries *series);
    void showToolTip(const QPointF &point);
    void setupClickEvent(QLineSeries *series);
    void onPointClicked(QPointF point);

    // Helper methods to get compliance and safety threshold
    QString getComplianceStatus(double pollutantLevel);
    QString getSafetyThreshold(double pollutantLevel);
};

#endif
