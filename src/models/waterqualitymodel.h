#ifndef WATERQUALITYMODEL_H
#define WATERQUALITYMODEL_H

#include <QAbstractTableModel>
#include <vector>

class WaterQualityModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit WaterQualityModel(QObject *parent = nullptr);

    // TODO: Implement model methods

private:
    // TODO: Add data storage
};

#endif // WATERQUALITYMODEL_H
