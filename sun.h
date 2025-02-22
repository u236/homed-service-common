#ifndef SUN_H
#define SUN_H

#define ANGLE   90.833

#include <math.h>
#include <QDate>

class Sun
{

public:

    Sun(double latitude, double longitude) :
        m_latitude(latitude), m_longitude(longitude), m_julianDay(0), m_offset(0) {}

    inline void setDate(const QDate &value) { m_julianDay = value.toJulianDay(); }
    inline void setOffset(int value) { m_offset = value / 60; }

    inline QTime sunrise(void) { return m_sunrise; }
    inline QTime sunset(void) { return m_sunset; }

    void updateSunrise(void);
    void updateSunset(void);

    QTime fromString(const QString &string);

private:

    double m_latitude, m_longitude, m_julianDay, m_offset;
    QTime m_sunrise, m_sunset;

    inline double radian(double value) { return value / 180 * M_PI; }
    inline double degree(double value) { return value * 180 / M_PI; }

    double julianDay(double century);
    double julianCentury(double day);

    double meanSunAnomaly(double t);
    double meanSunLongitude(double t);
    double sunDeclination(double t);
    double obliquityCorrection(double t);
    double timeEquation(double t);

    double sunriseHourAngle(double latitude, double declination, double offset);
    double sunsetHourAngle(double latitude, double declination, double offset);

};

#endif
