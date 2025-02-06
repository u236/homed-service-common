#include "expose.h"

void ExposeObject::registerMetaTypes(void)
{
    qRegisterMetaType <BinaryObject>        ("binaryExpose");
    qRegisterMetaType <SensorObject>        ("sensorExpose");
    qRegisterMetaType <ToggleObject>        ("toggleExpose");
    qRegisterMetaType <NumberObject>        ("numberExpose");
    qRegisterMetaType <SelectObject>        ("selectExpose");
    qRegisterMetaType <ButtonObject>        ("buttonExpose");
    qRegisterMetaType <SwitchObject>        ("switchExpose");
    qRegisterMetaType <LightObject>         ("lightExpose");
    qRegisterMetaType <CoverObject>         ("coverExpose");
    qRegisterMetaType <LockObject>          ("lockExpose");
    qRegisterMetaType <ThermostatObject>    ("thermostatExpose");
}

QJsonObject BinaryObject::request(void)
{
    QMap <QString, QVariant> options = option().toMap();
    QJsonObject json;

    if (m_name.startsWith("battery") || m_name.startsWith("tamper") || options.value("diagnostic").toBool())
        json.insert("entity_category", "diagnostic");

    if (options.contains("class"))
        json.insert("device_class", options.value("class").toString());

    if (options.contains("icon"))
        json.insert("icon", options.value("icon").toString());

    json.insert("force_update", true);
    json.insert("value_template", QString("{{ value_json.%1 }}").arg(m_name));
    json.insert("payload_on", true);
    json.insert("payload_off", false);
    json.insert("state_topic", m_stateTopic);

    return json;
}

QJsonObject SensorObject::request(void)
{
    QMap <QString, QVariant> options = option().toMap();
    QList <QString> valueTemplate = {QString("value_json.%1").arg(m_name)}, forceUpdate = {"action", "event", "scene"};
    QJsonObject json;

    for (int i = 0; i < forceUpdate.count(); i++)
    {
        if (!m_name.startsWith(forceUpdate.at(i)))
            continue;

        valueTemplate.append("is_defined");
        json.insert("force_update", true);
        break;
    }

    if (options.contains("round"))
        valueTemplate.append(QString("round(%1)").arg(options.value("round").toInt()));

    if (m_name.startsWith("battery") || options.value("diagnostic").toBool())
        json.insert("entity_category", "diagnostic");

    if (options.contains("class"))
        json.insert("device_class", options.value("class").toString());

    if (options.contains("state"))
        json.insert("state_class", options.value("state").toString());

    if (options.contains("unit"))
        json.insert("unit_of_measurement", options.value("unit").toString());

    if (options.contains("icon"))
        json.insert("icon", options.value("icon").toString());

    json.insert("value_template", QString("{{ %1 }}").arg(valueTemplate.join(" | ")));
    json.insert("state_topic", m_stateTopic);

    return json;
}

QJsonObject ToggleObject::request(void)
{
    QMap <QString, QVariant> options = option().toMap();
    QJsonObject json;

    if (!options.value("control").toBool())
        json.insert("entity_category", "config");

    if (options.contains("icon"))
        json.insert("icon", options.value("icon").toString());

    json.insert("value_template", QString("{{ value_json.%1 }}").arg(m_name));
    json.insert("state_on", true);
    json.insert("state_off", false);
    json.insert("state_topic", m_stateTopic);

    json.insert("payload_on", QString("{\"%1\":true}").arg(m_name));
    json.insert("payload_off", QString("{\"%1\":false}").arg(m_name));
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject NumberObject::request(void)
{
    QMap <QString, QVariant> options = option().toMap();
    QJsonObject json;

    if (!options.value("control").toBool())
        json.insert("entity_category", "config");

    if (options.contains("min"))
        json.insert("min", options.value("min").toDouble());

    if (options.contains("max"))
        json.insert("max", options.value("max").toDouble());

    if (options.contains("step"))
        json.insert("step", options.value("step").toDouble());

    if (options.contains("unit"))
        json.insert("unit_of_measurement", options.value("unit").toString());

    if (options.contains("icon"))
        json.insert("icon", options.value("icon").toString());

    json.insert("value_template", QString("{{ value_json.%1 }}").arg(m_name));
    json.insert("state_topic", m_stateTopic);

    json.insert("command_template", QString("{\"%1\":{{ value }}}").arg(m_name));
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject SelectObject::request(void)
{
    QMap <QString, QVariant> options = option().toMap();
    QJsonObject json;

    if (!options.value("control").toBool())
        json.insert("entity_category", "config");

    if (options.contains("icon"))
        json.insert("icon", options.value("icon").toString());

    json.insert("options", QJsonArray::fromStringList(options.value("enum").toStringList()));

    json.insert("value_template", QString("{{ value_json.%1 }}").arg(m_name));
    json.insert("state_topic", m_stateTopic);

    json.insert("command_template", QString("{\"%1\":\"{{ value }}\"}").arg(m_name));
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject ButtonObject::request(void)
{
    QMap <QString, QVariant> options = option().toMap();
    QJsonObject json;

    if (!options.value("control").toBool())
        json.insert("entity_category", "config");

    if (options.contains("icon"))
        json.insert("icon", options.value("icon").toString());

    json.insert("payload_press", QString("{\"%1\":true}").arg(m_name));
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject SwitchObject::request(void)
{
    QString name = QString(m_name).replace("switch", "status");
    QJsonObject json;

    json.insert("device_class", option().toString() == "outlet" ? "outlet" : "switch");

    json.insert("value_template", QString("{{ value_json.%1 }}").arg(name));
    json.insert("state_on", "on");
    json.insert("state_off", "off");
    json.insert("state_topic", m_stateTopic);

    json.insert("payload_on", QString("{\"%1\":\"on\"}").arg(name));
    json.insert("payload_off", QString("{\"%1\":\"off\"}").arg(name));
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject LightObject::request(void)
{
    QList <QString> options = option().toStringList();
    QString commandOnTemplate = "\"status\":\"on\"";
    QJsonObject json;

    if (options.contains("level"))
    {
        commandOnTemplate.append("{% if brightness is defined %},\"level\":{{ brightness }}{% endif %}");
        json.insert("brightness_template", "{{ value_json.level }}");
    }

    if (options.contains("color"))
    {
        commandOnTemplate.append("{% if red is defined and green is defined and blue is defined %},\"color\":[{{ red }},{{ green }},{{ blue }}]{% endif %}");
        json.insert("red_template", "{{ value_json.color[0] }}");
        json.insert("green_template", "{{ value_json.color[1] }}");
        json.insert("blue_template", "{{ value_json.color[2] }}");
    }

    if (options.contains("colorTemperature"))
    {
        QMap <QString, QVariant> colorTemperature = option("colorTemperature").toMap();

        commandOnTemplate.append("{% if color_temp is defined %},\"colorTemperature\":{{ color_temp }}{% endif %}");
        json.insert("color_temp_template", "{{ value_json.colorTemperature }}");
        json.insert("min_mireds", colorTemperature.value("min", 153).toInt());
        json.insert("max_mireds", colorTemperature.value("max", 500).toInt());
    }

    json.insert("schema", "template");

    json.insert("state_template", "{{ value_json.status }}");
    json.insert("state_topic", m_stateTopic);

    json.insert("command_on_template", QString("{%1}").arg(commandOnTemplate));
    json.insert("command_off_template", "{\"status\":\"off\"}");
    json.insert("payload_on", "on");
    json.insert("payload_off", "off");
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject CoverObject::request(void)
{
    QList <QString> list = m_name.split('_');
    QJsonObject json;
    QString id;

    if (list.count() > 1)
        id = QString("_%1").arg(list.at(1));

    json.insert("device_class", option().toString() == "blind" ? "blind" : "curtain");

    json.insert("value_template", QString("{{ value_json.cover%1 }}").arg(id));
    json.insert("state_open", "open");
    json.insert("state_closed", "closed");
    json.insert("state_topic", m_stateTopic);

    json.insert("position_template", QString("{{ value_json.position%1 }}").arg(id));
    json.insert("position_topic", m_stateTopic);

    json.insert("payload_open", QString("{\"cover%1\":\"open\"}").arg(id));
    json.insert("payload_close", QString("{\"cover%1\":\"close\"}").arg(id));
    json.insert("payload_stop", QString("{\"cover%1\":\"stop\"}").arg(id));
    json.insert("command_topic", m_commandTopic);

    json.insert("set_position_template", QString("{\"position%1\":{{ position }}}").arg(id));
    json.insert("set_position_topic", m_commandTopic);

    return json;
}

QJsonObject LockObject::request(void)
{
    QString name = QString(m_name).replace("lock", "status");
    QJsonObject json;

    if (option().toString() == "valve")
        json.insert("icon", "mdi:pipe-valve");

    json.insert("value_template", QString("{{ value_json.%1 }}").arg(name));
    json.insert("state_locked", "off");
    json.insert("state_unlocked", "on");
    json.insert("state_topic", m_stateTopic);

    json.insert("payload_lock", QString("{\"%1\":\"off\"}").arg(name));
    json.insert("payload_unlock", QString("{\"%1\":\"on\"}").arg(name));
    json.insert("command_topic", m_commandTopic);

    return json;
}

QJsonObject ThermostatObject::request(void)
{
    QList <QString> operationMode = option("operationMode").toMap().value("enum").toStringList(), fanMode = option("fanMode").toMap().value("enum").toStringList(), systemMode = option("systemMode").toMap().value("enum").toStringList();
    QMap <QString, QVariant> targetTemperature = option("targetTemperature").toMap();
    QJsonObject json;

    if (systemMode.contains("fan"))
        systemMode.replace(systemMode.indexOf("fan"), "fan_only");

    if (option("runningStatus").toBool())
    {
        QString actionTemplate = "\"off\" if value_json.running == false";

        if (systemMode.contains("heat"))
            actionTemplate.append(" else \"heating\" if value_json.systemMode == \"heat\"");

        if (systemMode.contains("cool"))
            actionTemplate.append(" else \"cooling\" if value_json.systemMode == \"cool\"");

        if (systemMode.contains("fan_only"))
            actionTemplate.append(" else \"fan\" if value_json.systemMode == \"fan\"");

        json.insert("action_template", QString("{{ %1 else \"idle\" }}").arg(actionTemplate));
        json.insert("action_topic", m_stateTopic);
    }

    if (!fanMode.isEmpty())
    {
        json.insert("fan_modes", QJsonArray::fromStringList(fanMode));

        json.insert("fan_mode_state_template", "{{ value_json.fanMode }}");
        json.insert("fan_mode_state_topic", m_stateTopic);

        json.insert("fan_mode_command_template", "{\"fanMode\":\"{{ value }}\"}");
        json.insert("fan_mode_command_topic", m_commandTopic);
    }

    if (!operationMode.isEmpty())
    {
        json.insert("preset_modes", QJsonArray::fromStringList(operationMode));

        json.insert("preset_mode_value_template", "{{ value_json.operationMode }}");
        json.insert("preset_mode_state_topic", m_stateTopic);

        json.insert("preset_mode_command_template", "{\"operationMode\":\"{{ value }}\"}");
        json.insert("preset_mode_command_topic", m_commandTopic);
    }

    if (targetTemperature.contains("min"))
        json.insert("min_temp", targetTemperature.value("min").toDouble());

    if (targetTemperature.contains("max"))
        json.insert("max_temp", targetTemperature.value("max").toDouble());

    if (targetTemperature.contains("step"))
        json.insert("temp_step", targetTemperature.value("step").toDouble());

    json.insert("modes", systemMode.isEmpty() ? QJsonArray {"heat"} : QJsonArray::fromStringList(systemMode));

    json.insert("mode_state_template", "{{ \"fan_only\" if value_json.systemMode == \"fan\" else value_json.systemMode }}");
    json.insert("mode_state_topic", m_stateTopic);

    json.insert("mode_command_template", "{\"systemMode\":\"{{ \"fan\" if value == \"fan_only\" else value }}\"}");
    json.insert("mode_command_topic", m_commandTopic);

    json.insert("current_temperature_template", "{{ value_json.temperature }}");
    json.insert("current_temperature_topic", m_stateTopic);

    json.insert("temperature_state_template", "{{ value_json.targetTemperature }}");
    json.insert("temperature_state_topic", m_stateTopic);

    json.insert("temperature_command_template", "{\"targetTemperature\":{{ value }}}");
    json.insert("temperature_command_topic", m_commandTopic);

    return json;
}
