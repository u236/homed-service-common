#ifndef HOMED_H
#define HOMED_H

#define MQTT_DEFAULT_QOS            0
#define MQTT_RECONNECT_INTERVAL     2000
#define STATUS_UPDATE_PERIOD        60000
#define EXIT_RESTART                1000

#include <QtMqtt/QMqttClient>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTimer>

class HOMEd : public QObject
{
    Q_OBJECT

public:

    HOMEd(const QString &configFile, bool multiple = false);

    inline QSettings *getConfig(void) { return m_config; }
    inline QString mqttPrefix(void) { return m_mqttPrefix; }
    inline QString serviceTopic(void) { return m_serviceTopic; }
    inline QString uniqueId(void) { return m_uniqueId; }    
    inline bool mqttStatus(void) { return m_connected; }

    void mqttSubscribe(const QString &topic);
    void mqttUnsubscribe(const QString &topic);

    void mqttPublish(const QString &topic, const QJsonObject &json, bool retain = false);
    void mqttPublishString(const QString &topic, const QString &message, bool retain = false);
    void mqttPublishDiscovery(const QString &name, const QString &version, const QString &haPrefix, bool permitJoin = false);
    void mqttPublishStatus(bool online = true);

    QString mqttTopic(const QString &topic = QString());

private:

    QString m_mqttPrefix, m_serviceTopic, m_uniqueId;
    bool m_connected;

    QMqttClient *m_mqtt;
    QElapsedTimer *m_elapsedTimer;
    QTimer *m_reconnectTimer;

    QSettings *m_config;
    QFileSystemWatcher *m_watcher;

public slots:

    virtual void quit(void);

private slots:

    virtual void mqttConnected(void) = 0;
    virtual void mqttReceived(const QByteArray &message, const QMqttTopicName &topic) = 0;

    void connected(void);
    void disconnected(void);
    void reconnect(void);
    void fileChanged(void);

};

#endif
