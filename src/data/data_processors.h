#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QSet>
#include "config.h"

class DataProcessor
{
private:
    QStringList pollutants;
    QSet<QString> pollutantSet;
    QMap<QString, QVector<QPair<double, int>>> locationResults;

    QString cleanField(const QString &field);
    int extractMonth(const QString &datetime);
    QStringList parseCSVLine(const QString &line);

public:
    DataProcessor();
    QMap<QString, QVector<QPair<double, int>>> processData();
};

#endif // DATA_PROCESSOR_H