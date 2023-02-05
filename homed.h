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
#include "logger.h"

class HOMEd : public QObject
{
    Q_OBJECT

public:

    HOMEd(void);

protected:

    inline QSettings *getConfig(void) { return m_config; }

    void mqttSubscribe(const QString &topic);
    void mqttUnsubscribe(const QString &topic);
    void mqttPublish(const QString &topic, const QJsonObject &json, bool retain = false);

    QString mqttTopic(const QString &topic = QString());

private:

    QString m_service, m_topicPrefix;

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

    void publishStatus(void);
    void mqttDisconnected(void);
    void mqttReconnect(void);
    void configChanged(void);

};

#endif
