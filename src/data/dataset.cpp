#include "dataset.hpp"
#include "csv.hpp"
#include <numeric>
#include <stdexcept>

void QuakeDataset::loadData(const std::string &filename)
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

// Results used in LitterIndicator pages
double QuakeDataset::sumResult(const QString &litterType, const QString &location, const QString &materialType) const
{
  if (data.empty())
  {
    return 0.0;
  }

  double sum = 0.0;

  for (const auto &measurement : data)
  {
    // Check if both location, litterType, and materialType match
    bool matchesLocation = (location == "All Locations") || (measurement.getLocation() == location.toStdString());
    bool matchesMaterialType = (materialType == "All Water Body Types") || (measurement.materialType() == materialType.toStdString());

    if (measurement.getPollutant() == litterType.toStdString() && matchesLocation && matchesMaterialType &&
        (measurement.getQualifier() != "<" && measurement.getQualifier() != ">"))
    {
      sum += measurement.getMagnitude();
    }
  }

  return sum;
}

double QuakeDataset::sumCompound(const QString &compoundType, const QString &location, const QString &time) const
{
  if (data.empty())
  {
    return 0.0; // Return 0 if no data is available
  }

  double sum = 0.0;

  for (const auto &measurement : data)
  {
    bool matchesLocation = (location == "All Locations") || (measurement.getLocation() == location.toStdString());
    bool matchesTime = (time == "All Times") || (QString::fromStdString(measurement.getTime()).startsWith(time));

    if (measurement.getPollutant() == compoundType.toStdString() && matchesLocation && matchesTime &&
        (measurement.getQualifier() != "<" || measurement.getQualifier() != ">"))
    {
      sum += measurement.getMagnitude();
    }
  }
  return sum;
}

double QuakeDataset::sumLitter(const QString &litterType, const QString &time) const
{
  if (data.empty())
  {
    return 0.0; // Return 0 if no data is available
  }

  double sum = 0.0;

  for (const auto &measurement : data)
  {
    bool matchesTime = (time == "All Times") || (QString::fromStdString(measurement.getTime()).startsWith(time));

    if (measurement.getPollutant() == litterType.toStdString() && matchesTime &&
        (measurement.getQualifier() != "<" || measurement.getQualifier() != ">"))
    {
      sum += measurement.getMagnitude();
    }
  }
  return sum;
}