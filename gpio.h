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

    static void setDirection(qint16 gpio, Direction direction);
    static void setStatus(qint16 gpio, bool status);
    static bool getStatus(qint16 gpio);

};

#endif
