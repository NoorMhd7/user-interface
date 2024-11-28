#include "examplepage.h"
#include <QVBoxLayout>
#include <QLabel>

ExamplePage::ExamplePage(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("This is the Example Page. Customize this template for other pages.", this);
    layout->addWidget(label);
    layout->setAlignment(label, Qt::AlignTop);
    setLayout(layout);
}
