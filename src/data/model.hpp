#ifndef MODEL_HPP
#define MODEL_HPP

#include <QAbstractTableModel>
#include "dataset.hpp"

class QuakeModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  int getBelowDetectionCount() const
  {
    int count = 0;
    for (size_t i = 0; i < dataset.size(); ++i)
    {
      if (dataset[i].getQualifier() == "<")
      {
        count++;
      }
    }
    return count;
  }
  explicit QuakeModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}
  QuakeModel(const QuakeModel &) = delete;            // Delete copy constructor
  QuakeModel &operator=(const QuakeModel &) = delete; // Delete copy assignment
  ~QuakeModel() override = default;

  void updateFromFile(const QString &);
  bool hasData() const { return dataset.size() > 0; }

  int rowCount(const QModelIndex & = QModelIndex()) const override { return dataset.size(); }
  int columnCount(const QModelIndex & = QModelIndex()) const override { return 8; }
  QVariant data(const QModelIndex &, int) const override;
  QVariant headerData(int, Qt::Orientation, int) const override;

  double meanDepth() const { return dataset.meanDepth(); }
  double meanMagnitude() const { return dataset.meanMagnitude(); }
  double sumResult(const QString &litterType, const QString &location, const QString &materialType) const { return dataset.sumResult(litterType, location, materialType); }
  double sumCompound(const QString &compoundType, const QString &location, const QString &time) const { return dataset.sumCompound(compoundType, location, time); }
  double sumLitter(const QString &litterType, const QString &time) const { return dataset.sumLitter(litterType, time); }

private:
  QuakeDataset dataset;
};

#endif