#ifndef STATS_HPP
#define STATS_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class StatsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit StatsDialog(QWidget *parent = nullptr);
  void update(double avgLevel, int totalMeasures, int belowLimit);

private:
  QLineEdit *avgPollutantLevel{nullptr};
  QLineEdit *totalMeasurements{nullptr};
  QLineEdit *belowDetection{nullptr};
  QPushButton *closeButton{nullptr};

  void createWidgets();
  void arrangeWidgets();
};

#endif