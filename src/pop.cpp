#include "pop.h"
#include <QVBoxLayout>
#include <QLabel>
// 1375 Location, around 500 pollutants
Pop::Pop(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("This is the pop page. Customize this template for other pages.", this);
    layout->addWidget(label);
    layout->setAlignment(label, Qt::AlignTop);
    setLayout(layout);
}