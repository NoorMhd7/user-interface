#include "dashboard.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Example card setup
    QLabel *title = new QLabel("Dashboard Overview", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(title);
    layout->setAlignment(title, Qt::AlignTop);

    setLayout(layout);
}
