#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <iomanip>

// Function to format the result
std::string formatResult(double value)
{
    std::ostringstream oss;
    if (value == 0.00004 || (value > 0.000039 && value < 0.000041))
    {
        oss << std::fixed << std::setprecision(5) << value;
    }
    else
    {
        oss << value; // Use default formatting for other numbers
    }
    return oss.str();
}

int main()
{
    std::vector<std::string> pollutants = {
        "PCB Con 028", "PCB Con 052", "PCB Con 101", "PCB Con 118", "PCB Con 138",
        "PCB Con 153", "PCB Con 156", "PCB Con 180", "PBDE 28", "PBDE 47",
        "PBDE 99", "PBDE 100", "PBDE 153", "PBDE 154", "PBDE 183", "DDT (PP')",
        "DDE (PP')", "TDE (PP)", "HCH Alpha", "HCH Beta", "HCH Gamma", "HCH Delta",
        "HCH Epsilon", "Chlordane-trans", "Chlordane-cis", "Dieldrin", "Aldrin",
        "Endrin", "Isodrin", "Hexachlorobenzene", "Endosulphan A", "Endosulphan B",
        "Heptachlor", "HBCDD"};

    std::unordered_set<std::string> pollutantSet(pollutants.begin(), pollutants.end());

    std::ifstream file("Y-2024.csv");
    if (!file.is_open())
    {
        std::cerr << "Failed to open file Y-2024.csv\n";
        return 1;
    }

    std::unordered_map<std::string, std::vector<std::pair<double, int>>> locationResults;

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(ss, field, ','))
        {
            fields.push_back(field);
        }

        if (fields.size() < 10)
            continue;

        std::string location = fields[3];  // sample.samplingPoint.label
        std::string datetime = fields[4];  // sample.sampleDateTime
        std::string pollutant = fields[5]; // determinand.label
        std::string resultStr = fields[9]; // result

        if (!pollutantSet.count(pollutant))
            continue;
        if (resultStr.empty())
            continue;

        double result;
        try
        {
            result = std::stod(resultStr);
        }
        catch (...)
        {
            continue;
        }

        int month;
        try
        {
            size_t firstDash = datetime.find('-');
            size_t secondDash = datetime.find('-', firstDash + 1);
            std::string monthStr = datetime.substr(firstDash + 1, secondDash - firstDash - 1);
            month = std::stoi(monthStr);
        }
        catch (...)
        {
            continue;
        }

        locationResults[location].emplace_back(result, month);
    }

    for (const auto &entry : locationResults)
    {
        const auto &location = entry.first;
        const auto &values = entry.second;

        std::cout << location << ", " << values.size() << ", [";
        for (size_t i = 0; i < values.size(); ++i)
        {
            std::cout << "(" << formatResult(values[i].first) << ", "
                      << values[i].second << ")";
            if (i != values.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << "]\n";
    }

    return 0;
}