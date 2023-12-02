#include "endpoint.h"
#include "expose.h"

void AbstractDeviceObject::publishExposes(HOMEd *controller, const QString &address, const QString uniqueId, bool remove)
{
    bool names = controller->getConfig()->value("mqtt/names", false).toBool(), legacy = controller->getConfig()->value("homeassistant/legacy", true).toBool();
    QString prefix = controller->getConfig()->value("homeassistant/prefix", "homeassistant").toString();
    QMap <QString, QVariant> data, endpointName = m_options.value("endpointName").toMap();

    for (auto it = m_endpoints.begin(); it != m_endpoints.end(); it++)
    {
        AbstractEndpointObject *endpoint = reinterpret_cast <AbstractEndpointObject*> (it.value().data());

        for (int i = 0; i < endpoint->exposes().count(); i++)
        {
            const Expose &expose = endpoint->exposes().at(i);
            QVariant option = expose->option();

            if (controller->getConfig()->value("homeassistant/enabled", false).toBool() && expose->homeassistant())
            {
                QString id = expose->multiple() ? QString::number(it.key()) : QString(), topic = names ? m_name : address;
                QList <QString> object = {expose->name()};
                QJsonObject json, identity;
                QJsonArray availability;

                if (!id.isEmpty())
                {
                    topic.append("/").append(id);
                    object.append(id);
                }

                if (!remove)
                {
                    QString name = expose->name();

                    name.replace(QRegExp("([A-Z0-9])"), " \\1").replace(0, 1, name.at(0).toUpper());

                    if (!id.isEmpty())
                        name.append(" ").append(endpointName.contains(id) ? endpointName.value(id).toString() : id);

                    expose->setStateTopic(controller->mqttTopic("fd/%1/%2").arg(controller->serviceName(), topic));
                    expose->setCommandTopic(controller->mqttTopic("td/%1/%2").arg(controller->serviceName(), topic));

                    json = expose->request();

                    identity.insert("identifiers", QJsonArray {QString(uniqueId)});
                    identity.insert("name", m_name);
                    identity.insert("model", m_description);

                    availability.append(QJsonObject {{"topic", controller->mqttTopic("device/%1/%2").arg(controller->serviceName(), names ? m_name : address)}, {"value_template", "{{ value_json.status }}"}});
                    availability.append(QJsonObject {{"topic", controller->mqttTopic("service/%1").arg(controller->serviceName())}, {"value_template", "{{ value_json.status }}"}});

                    json.insert("availability", availability);
                    json.insert("availability_mode", "all");
                    json.insert("device", identity);
                    json.insert("has_entity_name", legacy ? false : true);
                    json.insert("name", legacy ? QString("%1 %2").arg(m_name, name) : name);
                    json.insert("unique_id", QString("%1_%2").arg(uniqueId, object.join('_')));
                }

                controller->mqttPublish(QString("%1/%2/%3/%4/config").arg(prefix, expose->component(), uniqueId, object.join('_')), json, true);

                if (expose->name() == "action" || expose->name() == "event" || expose->name() == "scene")
                {
                    QList <QString> list = expose->name() != "scene" ? option.toStringList() : QVariant(option.toMap().value("name").toMap().values()).toStringList();

                    for (int i = 0; i < list.count(); i++)
                    {
                        QString subtype = list.at(i);
                        QList <QString> event = {subtype};
                        QJsonObject item;

                        subtype.replace(QRegExp("([A-Z])"), " \\1").replace(0, 1, subtype.at(0).toUpper());

                        if (!id.isEmpty())
                        {
                            if (endpointName.contains(id))
                                subtype.prepend(" ").prepend(endpointName.value(id).toString());
                            else
                                subtype.append(" ").append(id);

                            event.append(id);
                        }

                        if (!remove)
                        {
                            item.insert("automation_type", "trigger");
                            item.insert("device", identity);
                            item.insert("payload", event.at(0));
                            item.insert("subtype", subtype);
                            item.insert("topic", controller->mqttTopic("fd/%1/%2").arg(controller->serviceName(), topic));
                            item.insert("type", expose->name());
                            item.insert("value_template", QString("{{ value_json.%1 }}").arg(expose->name()));
                        }

                        controller->mqttPublish(QString("%1/device_automation/%2/%3/config").arg(prefix, uniqueId, event.join('_')), item, true);
                    }
                }
            }

            if (!remove)
            {
                QString id = QString::number(it.key()), key = expose->multiple() ? id : "common";
                QMap <QString, QVariant> map = data.value(key).toMap(), options = map.value("options").toMap();
                QList <QString> items = map.value("items").toStringList();

                items.append(expose->name());
                map.insert("items", QVariant(items));

                if (expose->name() == "light" && option.toStringList().contains("colorTemperature"))
                {
                    QVariant colorTemperature = expose->option("colorTemperature");

                    if (colorTemperature.isValid())
                        options.insert("colorTemperature", colorTemperature);
                }

                if (expose->name() == "thermostat")
                {
                    QVariant heatingStatus = expose->option("heatingStatus"), operationMode = expose->option("operationMode"), systemMode = expose->option("systemMode"), targetTemperature = expose->option("targetTemperature");

                    if (heatingStatus.isValid())
                        options.insert("heatingStatus", heatingStatus);

                    if (operationMode.isValid())
                        options.insert("operationMode", operationMode);

                    if (systemMode.isValid())
                        options.insert("systemMode", systemMode);

                    if (targetTemperature.isValid())
                        options.insert("targetTemperature", targetTemperature);
                }

                if (option.isValid())
                    options.insert(expose->name(), option);

                if (expose->multiple() && endpointName.contains(id))
                    options.insert("name", endpointName.value(id));

                if (!options.isEmpty())
                    map.insert("options", options);

                data.insert(key, map);
            }
        }
    }

    controller->mqttPublish(controller->mqttTopic("expose/%1/%2").arg(controller->serviceName(), names ? m_name : address), QJsonObject::fromVariantMap(data), true);
}

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
    return reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->modelName();
}

QMap <QString, QVariant> &AbstractMetaObject::meta(void)
{
    return m_parent->meta();
}
