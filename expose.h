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

    BinaryObject(const QString &name = "alarm") : ExposeObject(name, "binary_sensor") {}
    QJsonObject request(void) override;

};

class SensorObject : public ExposeObject
{

public:

    SensorObject(const QString &name, const QString &unit, quint8 round) : ExposeObject(name, "sensor"), m_unit(unit), m_round(round), m_custom(false) {}
    SensorObject(const QString &name, bool custom) : ExposeObject(name, "sensor"), m_round(0), m_custom(custom) {}
    QJsonObject request(void) override;

private:

    QString m_unit;
    quint8 m_round;
    bool m_custom;

};

class BooleanObject : public ExposeObject
{

public:

    BooleanObject(const QString &name) : ExposeObject(name, "switch") {}
    QJsonObject request(void) override;

};

class NumberObject : public ExposeObject
{

public:

    NumberObject(const QString &name) : ExposeObject(name, "number") {}
    QJsonObject request(void) override;

};

class SelectObject : public ExposeObject
{

public:

    SelectObject(const QString &name) : ExposeObject(name, "select") {}
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

namespace Binary
{
    class Contact : public BinaryObject
    {

    public:

        Contact(void) : BinaryObject("contact") {}

    };

    class BatteryLow : public BinaryObject
    {

    public:

        BatteryLow(void) : BinaryObject("batteryLow") {}

    };

    class Gas : public BinaryObject
    {

    public:

        Gas(void) : BinaryObject("gas") {}

    };

    class Occupancy : public BinaryObject
    {

    public:

        Occupancy(void) : BinaryObject("occupancy") {}

    };

    class Smoke : public BinaryObject
    {

    public:

        Smoke(void) : BinaryObject("smoke") {}

    };

    class Tamper : public BinaryObject
    {

    public:

        Tamper(void) : BinaryObject("tamper") {}

    };

    class WaterLeak : public BinaryObject
    {

    public:

        WaterLeak(void) : BinaryObject("waterLeak") {}

    };

    class Vibration : public BinaryObject
    {

    public:

        Vibration(void) : BinaryObject("vibration") {}

    };
}

namespace Sensor
{
    class Battery : public SensorObject
    {

    public:

        Battery(void) : SensorObject("battery", "%", 1) {}

    };

    class Temperature : public SensorObject
    {

    public:

        Temperature(void) : SensorObject("temperature", "°C", 1) {}

    };

    class Pressure : public SensorObject
    {

    public:

        Pressure(void) : SensorObject("pressure", "kPa", 1) {}

    };

    class Humidity : public SensorObject
    {

    public:

        Humidity(void) : SensorObject("humidity", "%", 1) {}

    };

    class Moisture : public SensorObject
    {

    public:

        Moisture(void) : SensorObject("moisture", "%", 1) {}

    };

    class Illuminance : public SensorObject
    {

    public:

        Illuminance(void) : SensorObject("illuminance", "lx", 0) {}

    };

    class CO2 : public SensorObject
    {

    public:

        CO2(void) : SensorObject("co2", "ppm", 0) {}

    };

    class ECO2 : public SensorObject
    {

    public:

        ECO2(void) : SensorObject("eco2", "ppm", 0) {}

    };

    class VOC : public SensorObject
    {

    public:

        VOC(void) : SensorObject("voc", "ppb", 0) {}

    };

    class Formaldehyde : public SensorObject
    {

    public:

        Formaldehyde(void) : SensorObject("formaldehyde", "µg/m³", 0) {}

    };

    class Frequency : public SensorObject
    {

    public:

        Frequency(void) : SensorObject("frequency", "Hz", 1) {}

    };

    class Voltage : public SensorObject
    {

    public:

        Voltage(void) : SensorObject("voltage", "V", 1) {}

    };

    class Current : public SensorObject
    {

    public:

        Current(void) : SensorObject("current", "A", 3) {}

    };

    class Power : public SensorObject
    {

    public:

        Power(void) : SensorObject("power", "W", 2) {}

    };

    class Energy : public SensorObject
    {

    public:

        Energy(void) : SensorObject("energy", "kWh", 2) {}

    };

    class TargetDistance : public SensorObject
    {

    public:

        TargetDistance(void) : SensorObject("targetDistance", "m", 1) {}

    };

    class Position : public SensorObject
    {

    public:

        Position(void) : SensorObject("position", false) {}

    };

    class Action : public SensorObject
    {

    public:

        Action(void) : SensorObject("action", false) {}

    };

    class Event : public SensorObject
    {

    public:

        Event(void) : SensorObject("event", false) {}

    };

    class Scene : public SensorObject
    {

    public:

        Scene(void) : SensorObject("scene", false) {}

    };
}

#endif
