#ifndef EXPOSE_H
#define EXPOSE_H

#include <QJsonArray>
#include <QJsonObject>
#include "endpoint.h"

class ExposeObject;
typedef QSharedPointer <ExposeObject> Expose;

class ExposeObject : public AbstractMetaObject
{

public:

    ExposeObject(const QString &name, const QString &component) :
        AbstractMetaObject(name), m_component(component), m_multiple(false), m_homeassistant(true) {}

    ExposeObject(const QString &name) :
        AbstractMetaObject(name), m_multiple(false), m_homeassistant(false) {}

    virtual ~ExposeObject(void) {}
    virtual QJsonObject request(void) { return QJsonObject(); };

    inline QString component(void) { return m_component; }

    inline void setStateTopic(const QString &value) { m_stateTopic = value; }
    inline void setCommandTopic(const QString &value) { m_commandTopic = value; }

    inline bool multiple(void) { return m_multiple; }
    inline void setMultiple(bool value) { m_multiple = value; }

    inline bool homeassistant(void) { return m_homeassistant; }
    static void registerMetaTypes(void);

protected:

    QString m_component, m_stateTopic, m_commandTopic;
    bool m_multiple, m_homeassistant;

};

class BinaryObject : public ExposeObject
{

public:

    BinaryObject(const QString &name = "binary") : ExposeObject(name, "binary_sensor") {}
    QJsonObject request(void) override;

};

class SensorObject : public ExposeObject
{

public:

    SensorObject(const QString &name = "sensor") : ExposeObject(name, "sensor") {}
    QJsonObject request(void) override;

};

class ToggleObject : public ExposeObject
{

public:

    ToggleObject(const QString &name = "toggle") : ExposeObject(name, "switch") {}
    QJsonObject request(void) override;

};

class NumberObject : public ExposeObject
{

public:

    NumberObject(const QString &name = "number") : ExposeObject(name, "number") {}
    QJsonObject request(void) override;

};

class SelectObject : public ExposeObject
{

public:

    SelectObject(const QString &name = "select") : ExposeObject(name, "select") {}
    QJsonObject request(void) override;

};

class ButtonObject : public ExposeObject
{

public:

    ButtonObject(const QString &name = "button") : ExposeObject(name, "button") {}
    QJsonObject request(void) override;

};

class LightObject : public ExposeObject
{

public:

    LightObject(void) : ExposeObject("light", "light") {}
    QJsonObject request(void) override;

};

class SwitchObject : public ExposeObject
{

public:

    SwitchObject(void) : ExposeObject("switch", "switch") {}
    QJsonObject request(void) override;

};

class LockObject : public ExposeObject
{

public:

    LockObject(void) : ExposeObject("lock", "lock") {}
    QJsonObject request(void) override;

};

class CoverObject : public ExposeObject
{

public:

    CoverObject(void) : ExposeObject("cover", "cover") {}
    QJsonObject request(void) override;

};

class ThermostatObject : public ExposeObject
{

public:

    ThermostatObject(void) : ExposeObject("thermostat", "climate") {}
    QJsonObject request(void) override;

};

#endif
