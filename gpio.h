#ifndef GPIO_H
#define GPIO_H

#include <QtGlobal>

class GPIO
{

public:

    enum Direction
    {
        Input,
        Output
    };

    static void direction(const QString &gpio, Direction direction);
    static void setStatus(const QString &gpio, bool status);
    static bool getStatus(const QString &gpio);

};

#endif
