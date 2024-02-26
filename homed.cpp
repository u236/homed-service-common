#include <QFileInfo>
#include <QLockFile>
#include <QProcess>
#include <signal.h>
#include "homed.h"
#include "logger.h"

HOMEd::HOMEd(const QString &configFile) : QObject(nullptr), m_connected(false), m_mqtt(new QMqttClient(this)), m_elapsedTimer(new QElapsedTimer), m_reconnectTimer(new QTimer(this)), m_watcher(new QFileSystemWatcher(this))
{
    QDate date = QDate::currentDate();

    m_config = new QSettings(QFileInfo::exists(configFile) ? configFile : QString("/etc/homed/%1.conf").arg(QCoreApplication::applicationName()), QSettings::IniFormat, this);
    m_watcher->addPath(m_config->fileName());

    setLogEnabled(m_config->value("log/enabled", false).toBool());
    setLogFile(m_config->value("log/file", "/var/log/homed.log").toString());
    qInstallMessageHandler(logger);

    if (date > QDate(date.year(), 12, 23) || date < QDate(date.year(), 1, 15))
        logInfo << "Merry Christmas and a Happy New Year!" << "\xF0\x9F\x8E\x81\xF0\x9F\x8E\x84\xF0\x9F\x8D\xBA";

    m_serviceName = QCoreApplication::applicationName().split("-").last();
    m_topicPrefix = m_config->value("mqtt/prefix", "homed").toString();

    m_mqtt->setHostname(m_config->value("mqtt/host", "localhost").toString());
    m_mqtt->setPort(static_cast <quint16> (m_config->value("mqtt/port", 1883).toInt()));
    m_mqtt->setUsername(m_config->value("mqtt/username").toString());
    m_mqtt->setPassword(m_config->value("mqtt/password").toString());

    m_mqtt->setWillTopic(mqttTopic("service/%1").arg(m_serviceName));
    m_mqtt->setWillMessage(QJsonDocument(QJsonObject {{"status", "offline"}}).toJson(QJsonDocument::Compact));

    connect(m_mqtt, &QMqttClient::connected, this, &HOMEd::connected, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::disconnected, this, &HOMEd::disconnected, Qt::QueuedConnection);
    connect(m_mqtt, &QMqttClient::messageReceived, this, &HOMEd::mqttReceived, Qt::QueuedConnection);

    connect(m_reconnectTimer, &QTimer::timeout, this, &HOMEd::reconnect, Qt::QueuedConnection);
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &HOMEd::fileChanged, Qt::QueuedConnection);

    m_mqtt->connectToHost();
    m_elapsedTimer->start();
    m_reconnectTimer->setSingleShot(true);
}

void HOMEd::quit(void)
{
    logInfo << "Goodbye!";

    mqttPublishStatus(false);
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

void HOMEd::mqttPublishString(const QString &topic, const QString &message, bool retain)
{
    m_mqtt->publish(topic, message.toUtf8(), MQTT_DEFAULT_QOS, retain);
}

void HOMEd::mqttPublishStatus(bool online)
{
    mqttPublish(mqttTopic("service/%1").arg(m_serviceName), {{"status", online ? "online" : "offline"}}, true);
}

QString HOMEd::mqttTopic(const QString &topic)
{
    return QString("%1/%2").arg(m_topicPrefix, topic);
}

void HOMEd::connected(void)
{
    m_connected = true;
    logInfo << "MQTT connected";
    mqttConnected();
}

void HOMEd::disconnected(void)
{
    m_connected = false;
    logWarning << "MQTT disconnected";
    m_reconnectTimer->start(MQTT_RECONNECT_INTERVAL);
}

void HOMEd::reconnect(void)
{
    m_mqtt->connectToHost();
}

void HOMEd::fileChanged(void)
{
    logWarning << "Configuration file changed, restarting...";
    QCoreApplication::exit(EXIT_RESTART);
}
