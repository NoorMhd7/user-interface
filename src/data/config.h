// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    static const std::string CSV_FILE;
};

// config.cpp
#include "config.h"

const std::string Config::CSV_FILE = "Y-2024.csv";

#endif