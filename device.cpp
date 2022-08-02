#include "device.h"

DeviceList::DeviceList(void)
{
    m_busNames.insert("modbus", DeviceBus::Modbus);
    m_busNames.insert("onewire", DeviceBus::OneWire);
    m_busNames.insert("zigbee", DeviceBus::ZigBee);
    m_busNames.insert("miio", DeviceBus::MIIO);
}

Device DeviceList::add(const QString &busName, const QString &address)
{
    auto it = m_busNames.find(busName);

    if (it != m_busNames.end())
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

Device DeviceList::get(const QString &busName, const QString &address)
{
    auto it = m_busNames.find(busName);

    if (it != m_busNames.end())
        return findDevice(it.value(), address);

    return Device();
}

QString DeviceList::getBusName(DeviceBus bus)
{
    for (auto it = m_busNames.begin(); it != m_busNames.end(); it++)
        if (it.value() == bus)
            return it.key();

    return "undefined";
}

Device DeviceList::findDevice(DeviceBus bus, const QString &address)
{
    for (int i = 0; i < m_devices.count(); i++)
    {
        Device device = m_devices.value(i);

        if (device->bus() == bus && device->address() == address)
            return device;
    }

    return Device();
}
