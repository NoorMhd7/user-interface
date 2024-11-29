#include "LitterIndicators.h"
#include <QVBoxLayout>
#include <QLabel>

LitterIndicators::LitterIndicators(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("This is the Litter Indicators page. Customize this template for other pages.", this);
    layout->addWidget(label);
    layout->setAlignment(label, Qt::AlignTop);
    setLayout(layout);
}
