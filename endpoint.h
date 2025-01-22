#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QSharedPointer>
#include <QVariant>
#include "homed.h"

class ExposeObject;
typedef QSharedPointer <ExposeObject> Expose;

class EndpointObject;
typedef QSharedPointer <EndpointObject> Endpoint;

class DeviceObject;
typedef QSharedPointer <DeviceObject> Device;

enum class Availability
{
    Unknown,
    Online,
    Offline,
    Inactive
};

class AbstractEndpointObject : public QObject
{
    Q_OBJECT

public:

    AbstractEndpointObject(quint8 id, const Device &device) :
        QObject(nullptr), m_id(id), m_device(device) {}

    inline quint8 id(void) { return m_id; }
    inline Device device(void) { return m_device; }
    inline QMap <QString, QVariant> &meta(void) { return m_meta; }
    inline QList <Expose> &exposes(void) { return m_exposes; }

protected:

    quint8 m_id;
    QWeakPointer <DeviceObject> m_device;
    QMap <QString, QVariant> m_meta;
    QList <Expose> m_exposes;

};

class AbstractDeviceObject : public QObject
{
    Q_OBJECT

public:

    AbstractDeviceObject(const QString &name) :
        QObject(nullptr), m_version(0), m_name(name), m_active(true), m_discovery(true), m_cloud(true), m_availability(Availability::Unknown) {}

    inline quint8 version(void) { return m_version; }
    inline void setVersion(quint8 value) { m_version = value; }

    inline QString name(void) { return m_name; }
    inline void setName(const QString &value) { m_name = value; }

    inline QString manufacturerName(void) { return m_manufacturerName; }
    inline void setManufacturerName(const QString &value) { m_manufacturerName = value; }

    inline QString modelName(void) { return m_modelName; }
    inline void setModelName(const QString &value) { m_modelName = value; }

    inline QString description(void) { return m_description; }
    inline void setDescription(const QString &value) { m_description = value; }

    inline QString note(void) { return m_note; }
    inline void setNote(const QString &value) { m_note = value; }

    inline bool active(void) { return m_active; }
    inline void setActive(bool value) { m_active = value; }

    inline bool discovery(void) { return m_discovery; }
    inline void setDiscovery(bool value) { m_discovery = value; }

    inline bool cloud(void) { return m_cloud; }
    inline void setCloud(bool value) { m_cloud = value; }

    inline Availability availability(void) { return m_availability; }
    inline void setAvailability(Availability value) { m_availability = value; }

    inline QMap <QString, QVariant> &options(void) { return m_options; }
    inline QMap <quint8, Endpoint> &endpoints(void) { return m_endpoints; }

    void publishExposes(HOMEd *controller, const QString &address, const QString uniqueId, const QString haPrefix, bool haEnabled, bool names, bool remove = false);

protected:

    quint8 m_version;
    QString m_name, m_manufacturerName, m_modelName, m_description, m_note;
    bool m_active, m_discovery, m_cloud;

    Availability m_availability;

    QMap <QString, QVariant> m_options;
    QMap <quint8, Endpoint> m_endpoints;

};

class AbstractMetaObject
{

public:

    AbstractMetaObject(const QString name) : m_name(name), m_parent(nullptr) {}

    inline QString name(void) { return m_name; }
    inline void setName(const QString &value) { m_name = value; }
    inline void setParent(AbstractEndpointObject *value) { m_parent = value; }

    QVariant option(const QString &name = QString(), const QVariant &defaultValue = QVariant());

protected:

    QString m_name;
    AbstractEndpointObject *m_parent;

    quint8 version(void);

    QString manufacturerName(void);
    QString modelName(void);

    QVariant meta(const QString &key, const QVariant &defaultValue = QVariant());
    void setMeta(const QString &key, const QVariant &value);
    void clearMeta(const QString &key);

};

#endif
