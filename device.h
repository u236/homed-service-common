#ifndef DEVICE_H
#define DEVICE_H

#include <QSharedPointer>
#include <QVariant>

class DeviceObject;
typedef QSharedPointer <DeviceObject> Device;

enum class DeviceBus
{
    Modbus,
    OneWire,
    ZigBee,
    MIIO
};

class DeviceObject
{

public:

    DeviceObject(DeviceBus bus, const QString &address) : m_bus(bus), m_address(address) {}

    inline DeviceBus bus(void) { return m_bus; }
    inline QString address(void) { return m_address; }

    inline QString name(void) { return m_name.isEmpty() ? m_address : m_name; }
    inline void setName(QString value) { m_name = value; }

    inline QVariant status(void)  { return m_status; }
    inline void setStatus(QVariant value) { m_status = value; }

private:

    DeviceBus m_bus;
    QString m_address;
    QString m_name;
    QVariant m_status;

};

class DeviceList
{

public:

    DeviceList(void);

    inline void clear(void) { m_devices.clear(); }

    Device add(const QString &busName, const QString &address);
    Device get(const QString &busName, const QString &address);

    QString getBusName(DeviceBus bus);

private:

    QMap <QString, DeviceBus> m_busNames;
    QList <Device> m_devices;

    Device findDevice(DeviceBus bus, const QString &address);

};

#endif
