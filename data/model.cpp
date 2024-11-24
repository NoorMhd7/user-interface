#include "model.hpp"

void QuakeModel::updateFromFile(const QString &filename)
{
  beginResetModel();
  dataset.loadData(filename.toStdString());
  endResetModel();
}

QVariant QuakeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole)
  {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole)
  {
    Quake q = dataset[index.row()];
    switch (index.column())
    {
    case 0:
      return QString::fromStdString(q.getLocation());
    case 1:
      return QString::fromStdString(q.getTime()); // DateTime
    case 2:
      return QString::fromStdString(q.getPollutant());
    case 3:
      return QString::fromStdString(q.getQualifier());
    case 4:
      return QString::fromStdString(q.materialType());
    case 5:
      return QVariant(q.getEasting());
    case 6:
      return QVariant(q.getNorthing());
    case 7:
      return QVariant(q.getMagnitude()); // Value
    }
  }

  return QVariant();
}

QVariant QuakeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
  {
    return QVariant();
  }

  if (orientation == Qt::Vertical)
  {
    return QVariant(section + 1);
  }

  switch (section)
  {
  case 0:
    return QString("Location");
  case 1:
    return QString("Date/Time");
  case 2:
    return QString("Pollutant");
  case 3:
    return QString("Qualifier");
  case 4:
    return QString("Material");
  case 5:
    return QString("East Cord");
  case 6:
    return QString("North Cord");
  case 7:
    return QString("Value");
  default:
    return QVariant();
  }
}