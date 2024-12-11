#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>

struct PollutantData
{
    double totalConcentration = 0.0;
    int count = 0;
};

typedef std::map<std::string, std::map<std::string, std::map<std::string, PollutantData>>> DataMap;

void processCSV(const std::string &csvFile, DataMap &dataMap)
{
    std::ifstream file(csvFile);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << csvFile << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Read the header line

    std::map<std::string, int> columnIndices;
    std::istringstream headerStream(line);
    std::string columnName;
    int index = 0;
    while (std::getline(headerStream, columnName, ','))
    {
        columnIndices[columnName] = index;
        index++;
    }

    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);
        std::string value;
        std::vector<std::string> row;

        while (std::getline(lineStream, value, ','))
        {
            row.push_back(value);
        }

        if (row.size() < 4)
            continue; // Skip incomplete rows

        std::string location = row[columnIndices["sample.samplingPoint.label"]];
        location = location.substr(1, location.size() - 2); // Remove the surrounding quotes
        std::string monthStr = row[columnIndices["sample.sampleDateTime"]];
        std::string month = monthStr.substr(5, 2);
        std::string pollutant = row[columnIndices["determinand.label"]];
        std::string concentrationStr = row[columnIndices["result"]];

        // Remove leading '>' or '<'
        size_t firstNonQuote = concentrationStr.find_first_not_of("><");
        if (firstNonQuote != std::string::npos)
        {
            concentrationStr = concentrationStr.substr(firstNonQuote);
        }

        try
        {
            double concentration = std::stod(concentrationStr);
            dataMap[location][month][pollutant].totalConcentration += concentration;
            dataMap[location][month][pollutant].count++;
        }
        catch (const std::invalid_argument &)
        {
            continue; // Skip rows with invalid concentration values
        }
    }

    file.close();
}

void writeFilteredData(const DataMap &dataMap, std::vector<std::string> &locations, std::vector<std::string> &pollutants, std::vector<std::map<std::string, std::vector<double>>> &chartData)
{
    locations.clear();
    pollutants.clear();
    chartData.clear();

    std::ofstream outFile("filtered_output.csv");
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open output file: filtered_output.csv" << std::endl;
        return;
    }

    outFile << "Location,Month,Pollutant,Concentration\n";

    for (const auto &locationEntry : dataMap)
    {
        const std::string &location = locationEntry.first;
        locations.push_back(location);

        for (const auto &monthEntry : locationEntry.second)
        {
            const std::string &month = monthEntry.first;
            for (const auto &pollutantEntry : monthEntry.second)
            {
                const std::string &pollutant = pollutantEntry.first;
                const PollutantData &data = pollutantEntry.second;

                if (data.count > 0)
                {
                    double average = data.totalConcentration / data.count;
                    outFile << location << "," << month << "," << pollutant << "," << std::fixed << std::setprecision(4) << average << "\n";

                    if (std::find(pollutants.begin(), pollutants.end(), pollutant) == pollutants.end())
                    {
                        pollutants.push_back(pollutant);
                    }
                }
            }
        }

        std::map<std::string, std::vector<double>> locationData;
        chartData.push_back(locationData);
    }

    outFile.close();
}

int main()
{
    std::string csvFile = "Y-2024.csv";
    DataMap dataMap;
    processCSV(csvFile, dataMap);

    std::vector<std::string> locations;
    std::vector<std::string> pollutants;
    std::vector<std::map<std::string, std::vector<double>>> chartData;
    writeFilteredData(dataMap, locations, pollutants, chartData);

    // Now you can use the locations, pollutants, and chartData in your application
    return 0;
}