#include <QFile>
#include "gpio.h"

void GPIO::direction(const QString &gpio, Direction direction)
{
    QFile file;
    bool check;
    int pin = gpio.toInt(&check);

    if (!check || pin < 0)
        return;

    file.setFileName("/sys/class/gpio/export");

    if (!file.open(QFile::WriteOnly))
        return;

    file.write(QString("%1\n").arg(pin).toUtf8());
    file.close();
    file.setFileName(QString("/sys/class/gpio/gpio%1/direction").arg(pin));

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

void GPIO::setStatus(const QString &gpio, bool status)
{
    QFile file;
    bool check;
    int pin = gpio.toInt(&check);

    if (check && pin < 0)
        return;

    file.setFileName(check ? QString("/sys/class/gpio/gpio%1/value").arg(pin) : gpio);

    if (!file.open(QFile::WriteOnly))
        return;

    file.write(status ? "1" : "0");
    file.close();
}

bool GPIO::getStatus(const QString &gpio)
{
    QFile file;
    bool check;
    int pin = gpio.toInt(&check);
    char status;

    if (check && pin < 0)
        return false;

    file.setFileName(check ? QString("/sys/class/gpio/gpio%1/value").arg(pin) : gpio);

    if (!file.open(QFile::ReadOnly))
        return false;

    file.read(&status, sizeof(status));
    file.close();

    return status != '0';
}
