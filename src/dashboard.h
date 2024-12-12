#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include "LitterIndicators.h"
#include "pop.h"
#include "Compound.h"
#include "PollutantOverview.h"

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

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void showHelp();
    void showUserGuide();
    void showCredits();

private:
    void addPollutantCard(const QString &title, const QString &summary, const QString &description, void (Dashboard::*slot)());
    void adjustCardLayout();
    int previousColumns = 0;

    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QGridLayout *contentLayout;
    QList<QFrame *> cards;

    LitterIndicators *litterIndicatorsPage;
    Pop *popPage;
    Compound *compoundPage;
    PollutantOverview *pollutantOverviewPage = nullptr;
};

#endif // DASHBOARD_H