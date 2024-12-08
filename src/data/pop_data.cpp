#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <iomanip>
#include "config.h"

class DataProcessor
{
private:
    std::vector<std::string> pollutants;
    std::unordered_set<std::string> pollutantSet;
    std::unordered_map<std::string, std::vector<std::pair<double, int>>> locationResults;

    // Helper function to clean and parse a CSV field
    std::string cleanField(const std::string &field)
    {
        std::string cleaned = field;
        cleaned.erase(0, cleaned.find_first_not_of(" \t\n\r\f\v\""));
        cleaned.erase(cleaned.find_last_not_of(" \t\n\r\f\v\"") + 1);
        return cleaned;
    }

    int extractMonth(const std::string &datetime)
    {
        size_t firstDash = datetime.find('-');
        if (firstDash != std::string::npos)
        {
            size_t secondDash = datetime.find('-', firstDash + 1);
            if (secondDash != std::string::npos)
            {
                std::string monthStr = datetime.substr(firstDash + 1, secondDash - firstDash - 1);
                try
                {
                    return std::stoi(monthStr);
                }
                catch (...)
                {
                    return -1;
                }
            }
        }
        return -1;
    }

    std::vector<std::string> parseCSVLine(const std::string &line)
    {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        std::stringstream fieldContent;

        for (char c : line)
        {
            if (c == '"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                fields.push_back(cleanField(fieldContent.str()));
                fieldContent.str("");
                fieldContent.clear();
            }
            else
            {
                fieldContent << c;
            }
        }
        fields.push_back(cleanField(fieldContent.str()));
        return fields;
    }

public:
    DataProcessor()
    {
        pollutants.push_back("PCB Con 028");
        pollutants.push_back("PCB Con 052");
        pollutants.push_back("PCB Con 101");
        pollutants.push_back("PCB Con 118");
        pollutants.push_back("PCB Con 138");
        pollutants.push_back("PCB Con 153");
        pollutants.push_back("PCB Con 156");
        pollutants.push_back("PCB Con 180");
        pollutants.push_back("PBDE 28");
        pollutants.push_back("PBDE 47");
        pollutants.push_back("PBDE 99");
        pollutants.push_back("PBDE 100");
        pollutants.push_back("PBDE 153");
        pollutants.push_back("PBDE 154");
        pollutants.push_back("PBDE 183");
        pollutants.push_back("DDT (PP')");
        pollutants.push_back("DDE (PP')");
        pollutants.push_back("TDE (PP)");
        pollutants.push_back("HCH Alpha");
        pollutants.push_back("HCH Beta");
        pollutants.push_back("HCH Gamma");
        pollutants.push_back("HCH Delta");
        pollutants.push_back("HCH Epsilon");
        pollutants.push_back("Chlordane-trans");
        pollutants.push_back("Chlordane-cis");
        pollutants.push_back("Dieldrin");
        pollutants.push_back("Aldrin");
        pollutants.push_back("Endrin");
        pollutants.push_back("Isodrin");
        pollutants.push_back("Hexachlorobenzene");
        pollutants.push_back("Endosulphan A");
        pollutants.push_back("Endosulphan B");
        pollutants.push_back("Heptachlor");
        pollutants.push_back("HBCDD");

        std::unordered_set<std::string> tempSet(pollutants.begin(), pollutants.end());
        pollutantSet.swap(tempSet);
    }

    void processData(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line))
        {
            std::vector<std::string> fields = parseCSVLine(line);

            // Make sure we have enough fields
            if (fields.size() < 12)
                continue;

            std::string location = cleanField(fields[3]);  // sample.samplingPoint.label
            std::string datetime = cleanField(fields[4]);  // sample.sampleDateTime
            std::string pollutant = cleanField(fields[5]); // determinand.label
            std::string resultStr = cleanField(fields[9]); // result

            if (pollutantSet.find(pollutant) == pollutantSet.end())
                continue;

            int month = extractMonth(datetime);
            if (month == -1)
                continue;

            // Skip empty results
            if (resultStr.empty())
                continue;

            // Try to parse the result
            double result;
            try
            {
                result = std::stod(resultStr);
                locationResults[location].push_back(std::make_pair(result, month));
            }
            catch (...)
            {
                continue;
            }
        }

        file.close();

        typedef std::unordered_map<std::string, std::vector<std::pair<double, int>>>::const_iterator MapIterator;

        for (MapIterator it = locationResults.begin(); it != locationResults.end(); ++it)
        {
            const std::string &location = it->first;
            const std::vector<std::pair<double, int>> &values = it->second;

            std::set<int> uniqueMonths;
            typedef std::vector<std::pair<double, int>>::const_iterator VectorIterator;

            for (VectorIterator valueIt = values.begin(); valueIt != values.end(); ++valueIt)
            {
                uniqueMonths.insert(valueIt->second);
            }

            if (uniqueMonths.size() > 1)
            {
                std::cout << location << ", " << values.size() << ", [";
                for (size_t i = 0; i < values.size(); ++i)
                {
                    std::cout << "("
                              << std::fixed
                              << std::setprecision(5)
                              << values[i].first << ", "
                              << values[i].second << ")";
                    if (i < values.size() - 1)
                    {
                        std::cout << ", ";
                    }
                }
                std::cout << "]\n";
            }
        }
    }
};

int main()
{
    DataProcessor processor;
    processor.processData(Config::CSV_FILE); // Use Config instead of hardcoded string
    return 0;
}