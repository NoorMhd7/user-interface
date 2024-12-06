#ifndef POP_H
#define POP_H

#include <QWidget>
#include <QtCharts>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
class QComboBox;
class QChartView;
QT_END_NAMESPACE

class Pop : public QWidget
{
    Q_OBJECT

public:
    explicit Pop(QWidget *parent = nullptr);

private slots:
    void onLocationChanged(int index);

private:
    QVBoxLayout *mainLayout;
    QComboBox *locationDropdown;
    QChartView *chartView;
};

#endif // POP_H