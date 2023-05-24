#include <math.h>
#include "sun.h"

QTime Sun::sunrise(void)
{
    double t = julianCentury(m_julianDay), n = julianCentury(julianDay(t) + (720 - (m_longitude + degrees(sunriseHourAngle(m_latitude, sunDeclination(t), ANGLE))) * 4 - timeEquation(t)) / 1440);
    double m = round(720 - (m_longitude + degrees(sunriseHourAngle(m_latitude, sunDeclination(n), ANGLE))) * 4 - timeEquation(n)) + m_offset;
    return QTime(static_cast <int> (m / 60), static_cast <int> (m) % 60);
}

QTime Sun::sunset(void)
{
    double t = julianCentury(m_julianDay), n = julianCentury(julianDay(t) + (720 - (m_longitude + degrees(sunsetHourAngle(m_latitude, sunDeclination(t), ANGLE))) * 4 - timeEquation(t)) / 1440.0);
    double m = round(720 - (m_longitude + degrees(sunsetHourAngle(m_latitude, sunDeclination(n), ANGLE))) * 4 - timeEquation(n)) + m_offset;
    return QTime(static_cast <int> (m / 60), static_cast <int> (m) % 60);
}

double Sun::radians(double degrees)
{
    return degrees * PI / 180;
}

double Sun::degrees(double radians)
{
    return radians * 180 / PI;
}

double Sun::julianCentury(double day)
{
    return (day - 2451545) / 36525;
}

double Sun::julianDay(double century)
{
    return century * 36525 + 2451545;
}

double Sun::meanSunAnomaly(double t)
{
    return 357.52911 + t * (35999.05029 - t * 0.0001537);
}

double Sun::meanSunLongitude(double t)
{
    return fmod(t * (36000.76983 + t * 0.0003032) + 280.46646, 360);
}

double Sun::sunDeclination(double t)
{
    double r = radians(meanSunAnomaly(t));
    double l = meanSunLongitude(t) + sin(r) * (1.914602 - t * (0.004817 + t * 0.000014)) + sin(r * 2) * (0.019993 - t * 0.000101) + sin(r * 3) * 0.000289 - sin(radians(125.04 - 1934.136 * t)) * 0.00478 - 0.00569;
    return degrees(asin(sin(radians(obliquityCorrection(t))) * sin(radians(l))));
}

double Sun::obliquityCorrection(double t)
{
    return cos(radians(125.04 - t * 1934.136)) * 0.00256 + (((21.448 - t * (46.8150 + t * (0.00059 - t * 0.001813))) / 60) + 26) / 60 + 23;
}

double Sun::timeEquation(double t)
{
    double e = 0.016708634 - t * (0.000042037 + t * 0.0000001267), y = pow(tan(radians(obliquityCorrection(t)) / 2), 2), a = meanSunAnomaly(t), l = meanSunLongitude(t), s = sin(radians(a));
    return degrees(y * sin(radians(l) * 2) - e * s * 2 + e * y * s * cos(radians(l) * 2) * 4 - pow(y, 2) * sin(radians(l) * 4) / 2 - pow(e, 2) * sin(radians(a) * 2) * 1.25) * 4;
}

double Sun::sunriseHourAngle(double latitude, double declination, double offset)
{
    double l = radians(latitude), s = radians(declination);
    return acos(cos(radians(offset)) / (cos(l) * cos(s)) - tan(l) * tan(s));
}

double Sun::sunsetHourAngle(double latitude, double declination, double offset)
{
    return 0 - sunriseHourAngle(latitude, declination, offset);
}
