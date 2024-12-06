#ifndef LitterIndicators_H2
#define LitterIndicators_H2

#include <QWidget>
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class LitterIndicators : public QWidget
{
    Q_OBJECT

public:
    explicit LitterIndicators(QWidget *parent = nullptr);
    QChartView *chartView;

private:
    void setupUI();
    void loadChartData();

    QVBoxLayout *mainLayout; // Layout for arranging widgets
    QComboBox *locationFilter;
    QComboBox *litterTypeFilter;
    QPushButton *refreshButton;
    QLabel *complianceOverview; 
};

#endif // EXAMPLEPAGE_H2
