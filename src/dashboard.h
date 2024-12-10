#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QGridLayout> // Include QGridLayout header

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

signals:
    void navigateToPollutantOverview();
    void navigateToPOPs();
    void navigateToLitterIndicators();
    void navigateToCompound();
    void navigateToData();

private slots:
    void showHelp();
    void showUserGuide();
    void showCredits();

private:
    void addPollutantCard(QGridLayout *layout, const QString &title, const QString &summary, int row, int col, void (Dashboard::*slot)());
};

#endif // DASHBOARD_H