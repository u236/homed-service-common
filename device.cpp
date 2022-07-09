#include "device.h"

DeviceList::DeviceList(void)
{
    m_typeNames.insert("modbus", DeviceType::Modbus);
    m_typeNames.insert("onewire", DeviceType::OneWire);
    m_typeNames.insert("zigbee", DeviceType::ZigBee);
    m_typeNames.insert("miio", DeviceType::MIIO);
}

Device DeviceList::add(const QString &typeName, const QString &address)
{
    auto it = m_typeNames.find(typeName);

    if (it != m_typeNames.end())
    {
        Device device = findDevice(it.value(), address);

        if (device.isNull())
        {
            device = Device(new DeviceObject(it.value(), address));
            m_devices.append(device);
        }

        return device;
    }

    return Device();
}

Device DeviceList::get(const QString &typeName, const QString &address)
{
    auto it = m_typeNames.find(typeName);

    if (it != m_typeNames.end())
        return findDevice(it.value(), address);

    return Device();
}

QString DeviceList::typeName(DeviceType type)
{
    for (auto it = m_typeNames.begin(); it != m_typeNames.end(); it++)
        if (it.value() == type)
            return it.key();

    return "undefined";
}

Device DeviceList::findDevice(DeviceType type, const QString &address)
{
    for (int i = 0; i < m_devices.count(); i++)
    {
        Device device = m_devices.value(i);

        if (device->type() == type && device->address() == address)
            return device;
    }

    return Device();
}
