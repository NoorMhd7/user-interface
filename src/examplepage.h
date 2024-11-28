#ifndef EXAMPLEPAGE_H
#define EXAMPLEPAGE_H

#include <QWidget>

class ExamplePage : public QWidget
{
    Q_OBJECT

public:
    explicit ExamplePage(QWidget *parent = nullptr);
};

#endif // EXAMPLEPAGE_H
