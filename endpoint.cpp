#include "endpoint.h"
#include "expose.h"
#include "parser.h"

void AbstractDeviceObject::publishExposes(HOMEd *controller, const QString &address, const QString uniqueId, const QString haPrefix, bool haEnabled, bool haUpdate, bool names, bool remove)
{
    QMap <QString, QVariant> endpointNames = m_options.value("endpointName").toMap(), data;
    QString deviceTopic = names ? m_name : address;
    QJsonObject identity;
    QJsonArray availability;

    if (m_discovery && haEnabled && !remove)
    {
        identity.insert("identifiers", QJsonArray {QString(uniqueId)});
        identity.insert("name", m_name);
        identity.insert("via_device", controller->uniqueId());

        if (!m_description.isEmpty())
            identity.insert("model", m_description);

        availability.append(QJsonObject {{"topic", controller->mqttTopic("device/%1/%2").arg(controller->serviceTopic(), deviceTopic)}, {"value_template", "{{ value_json.status }}"}});
        availability.append(QJsonObject {{"topic", controller->mqttTopic("service/%1").arg(controller->serviceTopic())}, {"value_template", "{{ value_json.status }}"}});
    }

    for (auto it = m_endpoints.begin(); it != m_endpoints.end(); it++)
    {
        AbstractEndpointObject *endpoint = reinterpret_cast <AbstractEndpointObject*> (it.value().data());

        for (int i = 0; i < endpoint->exposes().count(); i++)
        {
            const Expose &expose = endpoint->exposes().at(i);
            QString endpointId = expose->multiple() ? QString::number(it.key()) : QString(), endpointName = endpointNames.value(endpointId).toString();

            if (haEnabled && expose->discovery())
                publishDiscovery(controller, expose, identity, availability, deviceTopic, endpointId, endpointName, uniqueId, haPrefix, haUpdate, remove);

            if (remove)
                continue;

            addExposeData(expose, endpointId, endpointName, data);
        }
    }

    controller->mqttPublish(controller->mqttTopic("expose/%1/%2").arg(controller->serviceTopic(), deviceTopic), QJsonObject::fromVariantMap(data), true);
}

void AbstractDeviceObject::publishDiscovery(HOMEd *controller, const Expose &expose, const QJsonObject &identity, const QJsonArray &availability, const QString &deviceTopic, const QString &endpointId, const QString &endpointName, const QString &uniqueId, const QString &haPrefix, bool haUpdate, bool remove)
{
    QList <QString> list = {expose->name()}, trigger = {"action", "event", "scene"};
    QString title = expose->title(), topic = deviceTopic, object;
    QJsonObject json;

    if (!endpointId.isEmpty())
    {
        title = !endpointName.isEmpty() ? QString(endpointName).append(0x20).append(title) : title.append(0x20).append(endpointId);
        topic.append('/').append(endpointId);
        list.append(endpointId);
    }

    object = list.join('_');

    if (m_discovery && !remove)
    {
        expose->setStateTopic(controller->mqttTopic("fd/%1/%2").arg(controller->serviceTopic(), topic));
        expose->setCommandTopic(controller->mqttTopic("td/%1/%2").arg(controller->serviceTopic(), topic));

        json = expose->request();

        json.insert("availability", availability);
        json.insert("availability_mode", "all");

        if (haUpdate)
            json.insert("default_entity_id", QString("%1.%2_%3").arg(expose->component(), Parser::transliterate(m_name), title));

        json.insert("device", identity);
        json.insert("name", title);
        json.insert("unique_id", QString("%1_%2").arg(uniqueId, object));
    }

    controller->mqttPublish(QString("%1/%2/%3/%4/config").arg(haPrefix, expose->component(), uniqueId, object), json, true);

    if (!trigger.contains(expose->name().split('_').value(0)))
        return;

    publishTriggers(controller, expose, identity, availability, endpointId, endpointName, uniqueId, haPrefix, object, title, remove);
}

void AbstractDeviceObject::publishTriggers(HOMEd *controller, const Expose &expose, const QJsonObject &identity, const QJsonArray &availability, const QString &endpointId, const QString &endpointName, const QString &uniqueId, const QString &haPrefix, const QString &object, const QString &title, bool remove)
{
    QVariant data = expose->option().toMap().value("enum");
    QList <QString> items = data.type() == QVariant::Map ? QVariant(data.toMap().values()).toStringList() : data.toStringList(), list = expose->name().split('_');
    QJsonObject json;

    for (int i = 0; i < items.count(); i++)
    {
        QString subtype = items.at(i);
        QList <QString> event = {subtype};

        subtype.replace(QRegExp("([A-Z])"), " \\1").replace(0, 1, subtype.at(0).toUpper());

        if (!endpointId.isEmpty())
        {
            if (!endpointName.isEmpty())
                subtype.prepend(0x20).prepend(endpointName);
            else
                subtype.append(0x20).append(endpointId);

            event.append(endpointId);
        }

        if (list.count() > 1)
            event.append(list.value(1));

        if (m_discovery && !remove)
        {
            json.insert("automation_type", "trigger");
            json.insert("device", identity);
            json.insert("payload", event.at(0));
            json.insert("subtype", subtype);
            json.insert("topic", expose->stateTopic());
            json.insert("type", expose->name());
            json.insert("value_template", QString("{{ value_json.%1 }}").arg(expose->name()));
        }

        controller->mqttPublish(QString("%1/device_automation/%2/%3/config").arg(haPrefix, uniqueId, event.join('_')), json, true);
        json = QJsonObject();
    }

    if (m_discovery && !remove)
    {
        json.insert("availability", availability);
        json.insert("availability_mode", "all");
        json.insert("device", identity);
        json.insert("event_types", QJsonArray::fromStringList(items));
        json.insert("name", title);
        json.insert("state_topic", expose->stateTopic());
        json.insert("unique_id", QString("%1_%2").arg(uniqueId, object));
        json.insert("value_template", QString("{% if value_json.%1 is defined %}{\"event_type\":\"{{ value_json.%1 }}\"}{% endif %}").arg(expose->name()));
    }

    controller->mqttPublish(QString("%1/event/%2/%3/config").arg(haPrefix, uniqueId, object), json, true);
}

void AbstractDeviceObject::addExposeData(const Expose &expose, const QString &endpointId, const QString &endpointName, QMap <QString, QVariant> &data)
{
    QVariant option = expose->option();
    QString key = !endpointId.isEmpty() ? endpointId : "common", yandexType = expose->option("yandexType").toString();
    QMap <QString, QVariant> map = data.value(key).toMap(), options = map.value("options").toMap();
    QList <QString> items = map.value("items").toStringList();

    items.append(expose->name());
    map.insert("items", QVariant(items));

    if (expose->name().startsWith("light") && option.toStringList().contains("colorTemperature"))
    {
        QVariant colorTemperature = expose->option("colorTemperature");
        options.insert("colorTemperature", colorTemperature.isValid() ? colorTemperature : QMap <QString, QVariant> {{"min", 153}, {"max", 500}});
    }

    if (expose->name() == "thermostat")
    {
        static const QList <QString> exposes = {"systemMode", "operationMode", "targetTemperature", "runningStatus", "programTransitions", "programType"};

        for (int i = 0; i < exposes.count(); i++)
        {
            QVariant item = expose->option(exposes.at(i));

            if (!item.isValid())
                continue;

            options.insert(exposes.at(i), item);
        }
    }

    if (option.isValid())
        options.insert(expose->name(), option);

    if (expose->multiple() && !endpointName.isEmpty())
        options.insert("name", endpointName);

    if (!yandexType.isEmpty())
        options.insert("yandexType", yandexType);

    if (!options.isEmpty())
        map.insert("options", options);

    data.insert(key, map);
}

QVariant AbstractMetaObject::option(const QString &name, double defaultValue)
{
    QVariant value;

    if (m_parent)
    {
        AbstractDeviceObject *device = reinterpret_cast <AbstractDeviceObject*> (m_parent->device().data());
        QString optionName = name.isEmpty() ? m_name : name;
        QList <QString> list = optionName.split('_');

        if (list.count() < 2)
            optionName.append(QString("_%2").arg(m_parent->id()));

        value = device->options().contains(optionName) ? device->options().value(optionName) : device->options().value(list.at(0));
    }

    return value.isValid() ? value : !isnan(defaultValue) ? defaultValue : QVariant();
}

QVariant AbstractMetaObject::option(const QString &optionName, const QString &itemName, double defaultValue)
{
    QVariant value = option(optionName, defaultValue).toMap().value(itemName);
    return value.isValid() ? value : !isnan(defaultValue) ? defaultValue : QVariant();
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
