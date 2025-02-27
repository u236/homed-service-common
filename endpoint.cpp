#include "endpoint.h"
#include "expose.h"

void AbstractDeviceObject::publishExposes(HOMEd *controller, const QString &address, const QString uniqueId, const QString haPrefix, bool haEnabled, bool names, bool remove)
{
    QMap <QString, QVariant> data, endpointName = m_options.value("endpointName").toMap();
    QMap <QString, QString> abbreviation = {{"co2", "CO2"}, {"eco2", "eCO2"}, {"pm1", "PM1"}, {"pm10", "PM10"}, {"pm25", "PM2.5"}, {"voc", "VOC"}};
    QList <QString> trigger = {"action", "event", "scene"};

    for (auto it = m_endpoints.begin(); it != m_endpoints.end(); it++)
    {
        AbstractEndpointObject *endpoint = reinterpret_cast <AbstractEndpointObject*> (it.value().data());

        for (int i = 0; i < endpoint->exposes().count(); i++)
        {
            const Expose &expose = endpoint->exposes().at(i);
            QVariant option = expose->option();

            if (haEnabled && expose->discovery())
            {
                QString id = expose->multiple() ? QString::number(it.key()) : QString(), topic = names ? m_name : address, name = expose->name();
                QList <QString> object = {expose->name()};
                QJsonObject json, identity;
                QJsonArray availability;

                name = abbreviation.contains(name) ? abbreviation.value(name) : name.replace(QRegExp("([A-Z0-9])"), " \\1").replace(0, 1, name.at(0).toUpper());

                if (!id.isEmpty())
                {
                    topic.append('/').append(id);
                    name.append(0x20).append(endpointName.contains(id) ? endpointName.value(id).toString() : id);
                    object.append(id);
                }

                if (m_discovery && !remove)
                {
                    expose->setStateTopic(controller->mqttTopic("fd/%1/%2").arg(controller->serviceTopic(), topic));
                    expose->setCommandTopic(controller->mqttTopic("td/%1/%2").arg(controller->serviceTopic(), topic));

                    json = expose->request();

                    identity.insert("identifiers", QJsonArray {QString(uniqueId)});
                    identity.insert("name", m_name);
                    identity.insert("via_device", controller->uniqueId());

                    if (!m_description.isEmpty())
                        identity.insert("model", m_description);

                    availability.append(QJsonObject {{"topic", controller->mqttTopic("device/%1/%2").arg(controller->serviceTopic(), names ? m_name : address)}, {"value_template", "{{ value_json.status }}"}});
                    availability.append(QJsonObject {{"topic", controller->mqttTopic("service/%1").arg(controller->serviceTopic())}, {"value_template", "{{ value_json.status }}"}});

                    json.insert("availability", availability);
                    json.insert("availability_mode", "all");
                    json.insert("device", identity);
                    json.insert("name", name);
                    json.insert("unique_id", QString("%1_%2").arg(uniqueId, object.join('_')));
                }

                controller->mqttPublish(QString("%1/%2/%3/%4/config").arg(haPrefix, expose->component(), uniqueId, object.join('_')), json, true);

                if (trigger.contains(expose->name()))
                {
                    QVariant data = option.toMap().value("enum");
                    QList <QString> list = data.type() == QVariant::Map ? QVariant(data.toMap().values()).toStringList() : data.toStringList();

                    for (int i = 0; i < list.count(); i++)
                    {
                        QString subtype = list.at(i);
                        QList <QString> event = {subtype};

                        subtype.replace(QRegExp("([A-Z])"), " \\1").replace(0, 1, subtype.at(0).toUpper());
                        json = QJsonObject();

                        if (!id.isEmpty())
                        {
                            if (endpointName.contains(id))
                                subtype.prepend(0x20).prepend(endpointName.value(id).toString());
                            else
                                subtype.append(0x20).append(id);

                            event.append(id);
                        }

                        if (m_discovery && !remove)
                        {
                            json.insert("automation_type", "trigger");
                            json.insert("device", identity);
                            json.insert("payload", event.at(0));
                            json.insert("subtype", subtype);
                            json.insert("topic", controller->mqttTopic("fd/%1/%2").arg(controller->serviceTopic(), topic));
                            json.insert("type", expose->name());
                            json.insert("value_template", QString("{{ value_json.%1 }}").arg(expose->name()));
                        }

                        controller->mqttPublish(QString("%1/device_automation/%2/%3/config").arg(haPrefix, uniqueId, event.join('_')), json, true);
                    }

                    json = QJsonObject();

                    if (m_discovery && !remove)
                    {
                        json.insert("availability", availability);
                        json.insert("availability_mode", "all");
                        json.insert("device", identity);
                        json.insert("event_types", QJsonArray::fromStringList(list));
                        json.insert("name", name);
                        json.insert("state_topic", controller->mqttTopic("fd/%1/%2").arg(controller->serviceTopic(), topic));
                        json.insert("unique_id", QString("%1_%2").arg(uniqueId, object.join('_')));
                        json.insert("value_template", QString("{% if value_json.%1 is defined %}{\"event_type\":\"{{ value_json.%1 }}\"}{% endif %}").arg(expose->name()));
                    }

                    controller->mqttPublish(QString("%1/event/%2/%3/config").arg(haPrefix, uniqueId, object.join('_')), json, true);
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
                    options.insert("colorTemperature", colorTemperature.isValid() ? colorTemperature : QMap <QString, QVariant> {{"min", 153}, {"max", 500}});
                }

                if (expose->name() == "thermostat")
                {
                    QVariant systemMode = expose->option("systemMode"), operationMode = expose->option("operationMode"), targetTemperature = expose->option("targetTemperature"), runningStatus = expose->option("runningStatus"), programTransitions = expose->option("programTransitions");

                    if (systemMode.isValid())
                        options.insert("systemMode", systemMode);

                    if (operationMode.isValid())
                        options.insert("operationMode", operationMode);

                    if (targetTemperature.isValid())
                        options.insert("targetTemperature", targetTemperature);

                    if (runningStatus.isValid())
                        options.insert("runningStatus", runningStatus);

                    if (programTransitions.isValid())
                        options.insert("programTransitions", programTransitions);
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

    controller->mqttPublish(controller->mqttTopic("expose/%1/%2").arg(controller->serviceTopic(), names ? m_name : address), QJsonObject::fromVariantMap(data), true);
}

QVariant AbstractMetaObject::option(const QString &name, const QVariant &defaultValue)
{
    QVariant value;

    if (m_parent)
    {
        AbstractDeviceObject *device = reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data());
        QString optionName = name.isEmpty() ? m_name : name;
        QList <QString> list = optionName.split('_');

        if (list.count() == 1)
            optionName.append(QString("_%2").arg(m_parent->id()));

        value = device->options().contains(optionName) ? device->options().value(optionName) : device->options().value(list.at(0), defaultValue);
    }

    return value;
}

quint8 AbstractMetaObject::version(void)
{
    return m_parent ? reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->version() : 0;
}

QString AbstractMetaObject::manufacturerName(void)
{
    return m_parent ? reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->manufacturerName() : QString();
}

QString AbstractMetaObject::modelName(void)
{
    return m_parent ? reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data())->modelName() : QString();
}

QVariant AbstractMetaObject::meta(const QString &key, const QVariant &defaultValue)
{
    return m_parent ? m_parent->meta().value(key, defaultValue) : defaultValue;
}

void AbstractMetaObject::setMeta(const QString &key, const QVariant &value)
{
    if (!m_parent)
        return;

    m_parent->meta().insert(key, value);
}

void AbstractMetaObject::clearMeta(const QString &key)
{
    if (!m_parent)
        return;

     m_parent->meta().remove(key);
}
