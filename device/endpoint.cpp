#include "endpoint.h"

QVariant AbstractMetaObject::option(const QString &name, const QVariant &defaultValue)
{
    AbstractDeviceObject *device = reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data());
    QString optionName = name.isEmpty() ? m_name : name;
    QVariant value = device->options().value(QString("%1_%2").arg(optionName, QString::number(m_parent->id())));
    return value.isValid() ? value : device->options().value(optionName, defaultValue);
}

quint8 AbstractMetaObject::endpointId(void)
{
    return m_parent->id();
}

quint8 AbstractMetaObject::version(void)
{
    return reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->version();
}

QString AbstractMetaObject::manufacturerName(void)
{
    return reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->manufacturerName();
}

QString AbstractMetaObject::modelName(void)
{
    return reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->manufacturerName();
}
