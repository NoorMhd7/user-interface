#include "stats.hpp"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>

StatsDialog::StatsDialog(QWidget *parent)
    : QDialog(parent)
{
  createWidgets();
  arrangeWidgets();
  setWindowTitle("Water Quality Stats");
}

void StatsDialog::update(double avgLevel, int totalMeasures, int belowLimit)
{
  avgPollutantLevel->setText(QString::number(avgLevel, 'f', 4));
  totalMeasurements->setText(QString::number(totalMeasures));
  belowDetection->setText(QString::number(belowLimit));
}

void StatsDialog::createWidgets()
{
  avgPollutantLevel = new QLineEdit("?");
  avgPollutantLevel->setReadOnly(true);

  totalMeasurements = new QLineEdit("?");
  totalMeasurements->setReadOnly(true);

  belowDetection = new QLineEdit("?");
  belowDetection->setReadOnly(true);

  closeButton = new QPushButton("Close");
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void StatsDialog::arrangeWidgets()
{
  QFormLayout *form = new QFormLayout;
  form->addRow("Average Pollutant Level:", avgPollutantLevel);
  form->addRow("Total Measurements:", totalMeasurements);
  form->addRow("Below Detection Limit:", belowDetection);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(form);
  layout->addWidget(closeButton);

  setLayout(layout);
}