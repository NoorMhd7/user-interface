// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QPair>

class Config
{
public:
    static const QString UNIVERSAL_FILE_PATH;
    static const QString CSV_FILE;

    // Shared data structure for all classes
    typedef QMap<QString, QVector<QPair<double, int>>> LocationData;
    static LocationData processedData; // Stores the processed data that can be accessed by any class

    // Methods to access and update data
    static void updateProcessedData(const LocationData &data);
    static const LocationData &getProcessedData();
};

#endif // CONFIG_H