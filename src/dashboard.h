#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

private slots:
    void showHelp();
    void showUserGuide();
    void showCredits();
};

#endif // DASHBOARD_H