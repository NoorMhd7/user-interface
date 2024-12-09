#include "data_processors.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

DataProcessor::DataProcessor()
{
    pollutants = QStringList({"PCB Con 028", "PCB Con 052", "PCB Con 101", "PCB Con 118", "PCB Con 138",
                              "PCB Con 153", "PCB Con 156", "PCB Con 180", "PBDE 28", "PBDE 47",
                              "PBDE 99", "PBDE 100", "PBDE 153", "PBDE 154", "PBDE 183", "DDT (PP')",
                              "DDE (PP')", "TDE (PP)", "HCH Alpha", "HCH Beta", "HCH Gamma", "HCH Delta",
                              "HCH Epsilon", "Chlordane-trans", "Chlordane-cis", "Dieldrin", "Aldrin",
                              "Endrin", "Isodrin", "Hexachlorobenzene", "Endosulphan A", "Endosulphan B",
                              "Heptachlor", "HBCDD"});
    pollutantSet = QSet<QString>(pollutants.begin(), pollutants.end());
}

QString DataProcessor::cleanField(const QString &field)
{
    QString cleaned = field;
    cleaned = cleaned.trimmed();
    if (cleaned.startsWith('"') && cleaned.endsWith('"'))
    {
        cleaned = cleaned.mid(1, cleaned.length() - 2);
    }
    return cleaned;
}

int DataProcessor::extractMonth(const QString &datetime)
{
    QStringList parts = datetime.split('-');
    if (parts.size() >= 2)
    {
        bool ok;
        int month = parts[1].toInt(&ok);
        if (ok && month >= 1 && month <= 12)
        {
            return month;
        }
    }
    return -1;
}

QStringList DataProcessor::parseCSVLine(const QString &line)
{
    QStringList fields;
    QString field;
    bool inQuotes = false;
    QString fieldContent;

    for (const QChar &c : line)
    {
        if (c == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes)
        {
            fields.append(cleanField(fieldContent));
            fieldContent.clear();
        }
        else
        {
            fieldContent.append(c);
        }
    }
    fields.append(cleanField(fieldContent));
    return fields;
}

QMap<QString, QVector<QPair<double, int>>> DataProcessor::processData()
{
    QFile file(Config::CSV_FILE);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file:" << Config::CSV_FILE;
        return locationResults;
    }

    QTextStream in(&file);
    QString line = in.readLine(); // Skip header

    while (!in.atEnd())
    {
        line = in.readLine();
        QStringList fields = parseCSVLine(line);

        if (fields.size() < 12)
            continue;

        QString location = cleanField(fields[3]);  // sample.samplingPoint.label
        QString datetime = cleanField(fields[4]);  // sample.sampleDateTime
        QString pollutant = cleanField(fields[5]); // determinand.label
        QString resultStr = cleanField(fields[9]); // result

        if (!pollutantSet.contains(pollutant))
            continue;

        int month = extractMonth(datetime);
        if (month == -1)
            continue;

        if (resultStr.isEmpty())
            continue;

        bool ok;
        double result = resultStr.toDouble(&ok);
        if (!ok)
            continue;

        locationResults[location].append(qMakePair(result, month));
    }

    file.close();

    // Filter and process results
    QMap<QString, QVector<QPair<double, int>>> filteredResults;

    for (auto it = locationResults.begin(); it != locationResults.end(); ++it)
    {
        const QString &location = it.key();
        const QVector<QPair<double, int>> &values = it.value();

        // Get unique months
        QSet<int> uniqueMonths;
        for (const auto &value : values)
        {
            uniqueMonths.insert(value.second);
        }

        // Only include locations with measurements from different months
        if (uniqueMonths.size() > 1)
        {
            filteredResults[location] = values;
        }
    }

    // Update Config's shared data
    Config::updateProcessedData(filteredResults);

    return filteredResults;
}