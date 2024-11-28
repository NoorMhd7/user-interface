#include <stdexcept>
#include <sstream>
#include "quake.hpp"

using namespace std;

Quake::Quake(const string &location,
             const string &sampleTime,
             const string &pollutantName,
             const string &qualifierNotation,
             const string &material,
             double easting,
             double northing,
             double value)
    : location(location), time(sampleTime), pollutant(pollutantName), qualifier(qualifierNotation), material(material), easting(easting), northing(northing), magnitude(value)
{
  ostringstream error;

  // Basic validation
  if (value < -10000.0)
  {
    error << "Invalid measurement value: " << value;
    throw out_of_range(error.str());
  }
}

ostream &operator<<(ostream &out, const Quake &quake)
{
  return out << "\nLocation: " << quake.getLocation()
             << "\nTime: " << quake.getTime()
             << "\nPollutant: " << quake.getPollutant()
             << "\nQualifier: " << quake.getQualifier()
             << "\nMaterial Type: " << quake.materialType()
             << "\nEast: " << quake.getEasting()
             << "\nNorth: " << quake.getNorthing()
             << "\nValue: " << quake.getMagnitude() << endl;
}