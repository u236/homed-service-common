#ifndef SUN_H
#define SUN_H

#define PI      3.14159265358979323846264338327950288
#define ANGLE   90.833

#include <QDate>

class Sun
{

public:

    Sun(double latitude, double longitude)
        : m_latitude(latitude), m_longitude(longitude), m_julianDay(0), m_offset(0) {}

    inline void setDate(const QDate &value) { m_julianDay = value.toJulianDay(); }
    inline void setOffset(int value) { m_offset = value / 60; }

    QTime sunrise(void);
    QTime sunset(void);

private:

    double m_latitude, m_longitude, m_julianDay, m_offset;

    double radians(double degrees);
    double degrees(double radians);

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
