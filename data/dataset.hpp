#ifndef DATASET_HPP
#define DATASET_HPP

#include <vector>
#include <string>
#include "quake.hpp" // Make sure this path is correct

class QuakeDataset
{
public:
  void loadData(const std::string &filename);
  size_t size() const { return data.size(); }
  const Quake &operator[](size_t i) const { return data[i]; } // Return const reference

  double meanMagnitude() const;
  double meanDepth() const;

private:
  std::vector<Quake> data;
  void checkDataExists() const;
};

#endif