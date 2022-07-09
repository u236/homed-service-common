#ifndef DEVICE_H
#define DEVICE_H

#include <QSharedPointer>
#include <QVariant>

class DeviceObject;
typedef QSharedPointer <DeviceObject> Device;

enum class DeviceType
{
    Modbus,
    OneWire,
    ZigBee,
    MIIO
};

class DeviceObject
{

public:

    DeviceObject(DeviceType type, const QString &address) : m_type(type), m_address(address) {}

    inline DeviceType type(void) { return m_type; }
    inline QString address(void) { return m_address; }

    inline QString name(void) { return m_name.isEmpty() ? m_address : m_name; }
    inline void setName(QString value) { m_name = value; }

    inline QVariant status(void)  { return m_status; }
    inline void setStatus(QVariant value) { m_status = value; }

private:

    DeviceType m_type;
    QString m_address;
    QString m_name;
    QVariant m_status;

};

class DeviceList
{

public:

    DeviceList(void);

    inline void clear(void) { m_devices.clear(); }

    Device add(const QString &typeName, const QString &address);
    Device get(const QString &typeName, const QString &address);

    QString getTypeName(DeviceType type);

private:

    QMap <QString, DeviceType> m_typeNames;
    QList <Device> m_devices;

    Device findDevice(DeviceType type, const QString &address);

};

#endif
