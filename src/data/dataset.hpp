#ifndef DATASET_HPP
#define DATASET_HPP

#include <vector>
#include <string>
#include "quake.hpp" // Make sure this path is correct
#include <QString>
#include <QMessageBox>

class QuakeDataset
{
public:
  void loadData(const std::string &filename);
  size_t size() const { return data.size(); }
  const Quake &operator[](size_t i) const { return data[i]; } // Return const reference

  double meanMagnitude() const;
  double meanDepth() const;
  double sumResult(const QString &litterType, const QString &location, const QString &materialType) const;
  double sumCompound(const QString &compoundType, const QString &location, const QString &time) const;

private:
  std::vector<Quake> data;
  void checkDataExists() const;
};

#endif