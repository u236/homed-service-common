#include <QFileInfo>
#include <QLockFile>
#include <QProcess>
#include <signal.h>
#include "homed.h"

HOMEd::HOMEd(const QString &configFile) : QObject(nullptr), m_mqtt(new QMqttClient(this)), m_elapsedTimer(new QElapsedTimer), m_reconnectTimer(new QTimer(this)), m_watcher(new QFileSystemWatcher(this))
{
    m_config = new QSettings(QFileInfo::exists(configFile) ? configFile : QString("/etc/homed/%1.conf").arg(QCoreApplication::applicationName()), QSettings::IniFormat, this);
    m_watcher->addPath(m_config->fileName());

    m_service = QCoreApplication::applicationName().split("-").last();
    m_topicPrefix = m_config->value("mqtt/prefix", "homed").toString();

    m_mqtt->setHostname(m_config->value("mqtt/host", "localhost").toString());
    m_mqtt->setPort(static_cast <quint16> (m_config->value("mqtt/port", 1883).toInt()));
    m_mqtt->setUsername(m_config->value("mqtt/username").toString());
    m_mqtt->setPassword(m_config->value("mqtt/password").toString());

    m_mqtt->setWillTopic(mqttTopic("service/%1").arg(m_service));
    m_mqtt->setWillMessage(QJsonDocument(QJsonObject {{"status", "offline"}}).toJson(QJsonDocument::Compact));

    connect(m_mqtt, &QMqttClient::connected, this, &HOMEd::publishStatus, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::connected, this, &HOMEd::mqttConnected, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::messageReceived, this, &HOMEd::mqttReceived, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::disconnected, this, &HOMEd::mqttDisconnected, Qt::QueuedConnection);

    connect(m_reconnectTimer, &QTimer::timeout, this, &HOMEd::mqttReconnect, Qt::QueuedConnection);
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &HOMEd::configChanged, Qt::QueuedConnection);

    qInstallMessageHandler(logger);

    m_mqtt->connectToHost();
    m_elapsedTimer->start();
    m_reconnectTimer->setSingleShot(true);
}

void HOMEd::quit(void)
{
    logInfo << "Goodbye!";

    mqttPublish(mqttTopic("service/%1").arg(m_service), {{"status", "offline"}}, true);
    m_mqtt->disconnectFromHost();

    delete m_elapsedTimer;
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

QString HOMEd::mqttTopic(const QString &topic)
{
    return QString("%1/%2").arg(m_topicPrefix, topic);
}

void HOMEd::publishStatus(void)
{
    mqttPublish(mqttTopic("service/%1").arg(m_service), {{"status", "online"}}, true);
}

void HOMEd::mqttDisconnected(void)
{
    logWarning << "MQTT disconnected";
    m_reconnectTimer->start(MQTT_RECONNECT_INTERVAL);
}

void HOMEd::mqttReconnect(void)
{
    m_mqtt->connectToHost();
}

void HOMEd::configChanged(void)
{
    logWarning << "Config file changed, restarting";
    QCoreApplication::exit(EXIT_RESTART);
}
