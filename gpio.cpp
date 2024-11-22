#include <QFile>
#include "gpio.h"

void GPIO::direction(const QString &gpio, Direction direction)
{
    QList <QString> list = gpio.split('|');
    QFile file;
    bool check;
    int pin = list.value(0).toInt(&check);

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
    QList <QString> list = gpio.split('|');
    QFile file;
    bool check;
    int pin = list.value(0).toInt(&check);

    if (check && pin < 0)
        return;

    file.setFileName(check ? QString("/sys/class/gpio/gpio%1/value").arg(pin) : list.value(0));

    if (!file.open(QFile::WriteOnly))
        return;

    if (list.value(1) == "invert")
        status = status ? false : true;

    file.write(status ? "1" : "0");
    file.close();
}

bool GPIO::getStatus(const QString &gpio)
{
    QList <QString> list = gpio.split('|');
    QFile file;
    bool check;
    int pin = list.value(0).toInt(&check);
    char status;

    if (check && pin < 0)
        return false;

    file.setFileName(check ? QString("/sys/class/gpio/gpio%1/value").arg(pin) : list.value(0));

    if (!file.open(QFile::ReadOnly))
        return false;

    file.read(&status, sizeof(status));
    file.close();

    return list.value(1) == "invert" ? status == '0' : status != '0';
}
