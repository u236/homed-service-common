#include <QFileInfo>
#include <QLockFile>
#include <QProcess>
#include "homed.h"
#include "logger.h"

HOMEd::HOMEd(const QString &configFile, bool multiple, bool mqtt) : QObject(nullptr), m_mqtt(new QMqttClient(this)), m_statusTimer(new QTimer(this)), m_reconnectTimer(new QTimer(this)), m_watcher(new QFileSystemWatcher(this)), m_connected(false), m_first(true)
{
    QDate date = QDate::currentDate();
    QString instance;

    m_config = new QSettings(configFile.isEmpty() ? QString("/etc/homed/%1.conf").arg(QCoreApplication::applicationName()) : configFile, QSettings::IniFormat, this);
    m_watcher->addPath(m_config->fileName());

    setLogEnabled(m_config->value("log/enabled", false).toBool());
    setLogTimestams(m_config->value("log/timestamps", true).toBool());
    setLogFile(m_config->value("log/file", "/var/log/homed.log").toString());
    qInstallMessageHandler(logger);

    if (date > QDate(date.year(), 12, 23) || date < QDate(date.year(), 1, 15))
        logInfo << "Merry Christmas and a Happy New Year!" << "\xF0\x9F\x8E\x81\xF0\x9F\x8E\x84\xF0\x9F\x8D\xBA";

    m_mqttPrefix = m_config->value("mqtt/prefix", "homed").toString();
    m_interval = static_cast <quint32> (m_config->value("mqtt/interval").toInt() * 1000);
    instance = m_config->value("mqtt/instance").toString();

    m_serviceTopic = QCoreApplication::applicationName().split('-').last();
    m_uniqueId = QString("homed-%1_%2").arg(m_serviceTopic, m_mqttPrefix);

    if (multiple && !instance.isEmpty())
    {
        m_serviceTopic.append('/').append(instance);
        m_uniqueId.append('_').append(instance);
    }

    m_mqtt->setHostname(m_config->value("mqtt/host", "localhost").toString());
    m_mqtt->setPort(static_cast <quint16> (m_config->value("mqtt/port", 1883).toInt()));
    m_mqtt->setUsername(m_config->value("mqtt/username").toString());
    m_mqtt->setPassword(m_config->value("mqtt/password").toString());

    m_mqtt->setWillTopic(mqttTopic("service/%1").arg(m_serviceTopic));
    m_mqtt->setWillMessage(QJsonDocument(QJsonObject {{"status", "offline"}}).toJson(QJsonDocument::Compact));
    m_mqtt->setWillRetain(true);

    connect(m_mqtt, &QMqttClient::connected, this, &HOMEd::connected, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::disconnected, this, &HOMEd::disconnected, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::messageReceived, this, &HOMEd::mqttReceived, Qt::QueuedConnection);

    connect(m_reconnectTimer, &QTimer::timeout, this, &HOMEd::reconnect, Qt::QueuedConnection);
    connect(m_statusTimer, &QTimer::timeout, this, &HOMEd::publishStatus, Qt::QueuedConnection);
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &HOMEd::fileChanged, Qt::QueuedConnection);

    if (mqtt)
        m_mqtt->connectToHost();

    m_reconnectTimer->setSingleShot(true);
    m_statusTimer->setSingleShot(true);
}

void HOMEd::quit(void)
{
    logInfo << "Goodbye!";
    mqttPublishStatus(false);
    m_mqtt->disconnectFromHost();
}

void HOMEd::mqttSubscribe(const QString &topic)
{
    m_mqtt->subscribe(topic, MQTT_DEFAULT_QOS);
}

void HOMEd::mqttUnsubscribe(const QString &topic)
{
    m_mqtt->unsubscribe(topic);
}

void HOMEd::mqttPublish(const QString &topic, const QJsonObject &json, bool retain)
{
    m_mqtt->publish(topic, json.isEmpty() ? QByteArray() : QJsonDocument(json).toJson(QJsonDocument::Compact), MQTT_DEFAULT_QOS, retain);
}

void HOMEd::mqttPublishString(const QString &topic, const QString &message, bool retain)
{
    m_mqtt->publish(topic, message.toUtf8(), MQTT_DEFAULT_QOS, retain);
}

void HOMEd::mqttPublishDiscovery(const QString &name, const QString &version, const QString &haPrefix, bool permitJoin)
{
    QList <QString> list = {"connectivity", "lastSeen", "version", "permitJoin", "restartService"};
    QJsonObject identity;

    identity.insert("identifiers", QJsonArray {m_uniqueId});
    identity.insert("name", QString("HOMEd %1 (%2)").arg(name, m_mqttPrefix));
    identity.insert("model", QString("HOMEd %1 Service (%2)").arg(name, m_mqttPrefix));
    identity.insert("sw_version", version);

    for (int i = 0; i < list.count(); i++)
    {
        QString component, item = list.at(i);
        QJsonObject json;

        switch (i)
        {
            case 0: // connectivity
                component = "binary_sensor";
                json.insert("device_class", "connectivity");
                json.insert("state_topic", mqttTopic("service/%1").arg(m_serviceTopic));
                json.insert("value_template", "{{ value_json.status }}");
                json.insert("payload_on", "online");
                json.insert("payload_off", "offline");
                break;

            case 1: // lastSeen

                if (!m_interval)
                    continue;

                component = "sensor";
                json.insert("device_class", "timestamp");
                json.insert("icon", "mdi:clock");
                json.insert("state_topic", mqttTopic("service/%1").arg(m_serviceTopic));
                json.insert("value_template", "{{ value_json.timestamp | is_defined | timestamp_local }}");
                break;

            case 2: // version
                component = "sensor";
                json.insert("icon", "mdi:tag");
                json.insert("state_topic", mqttTopic("status/%1").arg(m_serviceTopic));
                json.insert("value_template", "{{ value_json.version }}");
                break;

            case 3: // permitJoin

                if (!permitJoin)
                    continue;

                component = "switch";
                json.insert("icon", "mdi:human-greeting");
                json.insert("state_topic", mqttTopic("status/%1").arg(m_serviceTopic));
                json.insert("command_topic", mqttTopic("command/%1").arg(m_serviceTopic));
                json.insert("value_template", "{{ value_json.permitJoin }}");
                json.insert("state_on", true);
                json.insert("state_off", false);
                json.insert("payload_on", "{\"action\": \"setPermitJoin\", \"enabled\": true}");
                json.insert("payload_off", "{\"action\": \"setPermitJoin\", \"enabled\": false}");
                break;

            case 4: // restartService
                component = "button";
                json.insert("icon", "mdi:restart");
                json.insert("command_topic", mqttTopic("command/%1").arg(m_serviceTopic));
                json.insert("payload_press", "{\"action\": \"restartService\"}");
                break;
        }

        if (i)
        {
            json.insert("availability_topic", mqttTopic("service/%1").arg(m_serviceTopic));
            json.insert("availability_template", "{{ value_json.status }}");
        }

        json.insert("device", identity);
        json.insert("entity_category", i < 3 ? "diagnostic" : "config");
        json.insert("name", QString(item).replace(QRegExp("([A-Z])"), " \\1").replace(0, 1, item.at(0).toUpper()));
        json.insert("unique_id", QString("%1_%2").arg(m_uniqueId, item));

        mqttPublish(QString("%1/%2/%3/%4/config").arg(haPrefix, component, m_uniqueId, item), json, true);
    }
}

void HOMEd::mqttPublishStatus(bool online)
{
    QJsonObject json = {{"status", online ? "online" : "offline"}};

    if (online && m_interval)
    {
        if (!m_statusTimer->isActive())
            m_statusTimer->start(m_interval);

        json.insert("timestamp", QDateTime::currentSecsSinceEpoch());
    }

    mqttPublish(mqttTopic("service/%1").arg(m_serviceTopic), json, true);
}

QString HOMEd::mqttTopic(const QString &topic)
{
    return QString("%1/%2").arg(m_mqttPrefix, topic);
}

void HOMEd::connected(void)
{
    m_connected = true;
    logInfo << "MQTT connected to" << QString("%1:%2").arg(m_mqtt->hostname()).arg(m_mqtt->port());
    mqttConnected();
}

void HOMEd::disconnected(void)
{
    m_reconnectTimer->start(MQTT_RECONNECT_INTERVAL);

    if (!m_connected && !m_first)
        return;

    m_connected = false;
    m_first = false;

    logWarning << "MQTT disconnected";
    mqttDisconnected();
}

void HOMEd::reconnect(void)
{
    m_mqtt->connectToHost();
}

void HOMEd::publishStatus(void)
{
    mqttPublishStatus();
}

void HOMEd::fileChanged(void)
{
    logWarning << "Configuration file changed, restarting...";
    QCoreApplication::exit(EXIT_RESTART);
}
