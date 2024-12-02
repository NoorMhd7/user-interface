#ifndef COMPOUND_H
#define COMPOUND_H

#include <QWidget>

class Compound : public QWidget
{
    Q_OBJECT

public:
    explicit Compound(QWidget *parent = nullptr);
};

#endif
