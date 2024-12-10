// config.cpp
#include "config.h"

const QString Config::UNIVERSAL_FILE_PATH = "../src/data/Y-2024.csv";
const QString Config::CSV_FILE = Config::UNIVERSAL_FILE_PATH;

Config::LocationData Config::processedData;

void Config::updateProcessedData(const LocationData &data)
{
    processedData = data;
}

const Config::LocationData &Config::getProcessedData()
{
    return processedData;
}