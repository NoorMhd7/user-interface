#ifndef QUAKE_HPP
#define QUAKE_HPP

#include <string>

class Quake
{
public:
  Quake() = default; // Add default constructor
  Quake(const std::string &location,
        const std::string &datetime,
        const std::string &pollutant,
        const std::string &qualifier,
        const std::string &material,
        double easting,
        double northing,
        double value);
  double getEasting() const { return easting; }
  double getNorthing() const { return northing; }
  std::string materialType() const { return material; }
  std::string getTime() const { return time; }
  std::string getLocation() const { return location; }
  std::string getPollutant() const { return pollutant; }
  std::string getQualifier() const { return qualifier; }
  double getMagnitude() const { return magnitude; }

private:
  double easting;
  double northing;
  std::string material;
  std::string location;
  std::string time;
  std::string pollutant;
  std::string qualifier;
  double magnitude;
};

#endif