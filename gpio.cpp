#include <QFile>
#include "gpio.h"

void GPIO::setDirection(qint16 gpio, Direction direction)
{
    QFile file;

    if (gpio < 0)
        return;

    file.setFileName("/sys/class/gpio/export");

    if (!file.open(QFile::WriteOnly))
        return;

    file.write(QString("%1\n").arg(gpio).toUtf8());
    file.close();
    file.setFileName(QString("/sys/class/gpio/gpio%1/direction").arg(gpio));

    if (!file.open(QFile::WriteOnly))
        return;

    switch (direction)
    {
        case Input:
            file.write("in");
            break;

        case Output:
            file.write("out");
            break;
    }

    file.close();
}

void GPIO::setStatus(qint16 gpio, bool status)
{
    QFile file;

    if (gpio < 0)
        return;

    file.setFileName(QString("/sys/class/gpio/gpio%1/value").arg(gpio));

    if (!file.open(QFile::WriteOnly))
        return;

    file.write(status ? "1" : "0");
    file.close();
}

bool GPIO::getStatus(qint16 gpio)
{
    QFile file;
    char status;

    if (gpio < 0)
        return false;

    file.setFileName(QString("/sys/class/gpio/gpio%1/value").arg(gpio));

    if (!file.open(QFile::ReadOnly))
        return false;

    file.read(&status, sizeof(status));
    file.close();

    return status != '0';
}
