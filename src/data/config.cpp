// config.cpp
#include "config.h"

const QString Config::CSV_FILE = "../src/data/Y-2024.csv";
Config::LocationData Config::processedData;

void Config::updateProcessedData(const LocationData &data)
{
    processedData = data;
}

const Config::LocationData &Config::getProcessedData()
{
    return processedData;
}