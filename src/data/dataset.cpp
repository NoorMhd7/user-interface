#include "dataset.hpp"
#include "csv.hpp"
#include <numeric>
#include <stdexcept>

void QuakeDataset::loadData(const std::string &filename = "Y-2024.csv")
{
  csv::CSVReader reader(filename);
  data.clear();

  for (const auto &row : reader)
  {
    try
    {
      Quake measurement(
          row["sample.samplingPoint.label"].get<std::string>(),
          row["sample.sampleDateTime"].get<std::string>(),
          row["determinand.label"].get<std::string>(),
          row["resultQualifier.notation"].get<std::string>(),
          row["sample.sampledMaterialType.label"].get<std::string>(),
          row["sample.samplingPoint.easting"].get<double>(),
          row["sample.samplingPoint.northing"].get<double>(),
          row["result"].get<double>());
      data.push_back(measurement);
    }
    catch (const std::exception &e)
    {
      // Skip invalid rows
      continue;
    }
  }
}

double QuakeDataset::meanMagnitude() const
{
  if (data.empty())
  {
    return 0.0;
  }

  double sum = 0.0;
  int count = 0;

  for (const auto &measurement : data)
  {
    if (measurement.getQualifier() != "<" || measurement.getQualifier() != ">")
    {
      sum += measurement.getMagnitude();
      count++;
    }
  }

  return count > 0 ? sum / count : 0.0;
}

void QuakeDataset::checkDataExists() const
{
  if (data.empty())
  {
    throw std::runtime_error("Dataset is empty");
  }
}