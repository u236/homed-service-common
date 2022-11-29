#include <math.h>
#include "color.h"

Color Color::fromCT(double ct)
{
    double r, g, b, k = 10000 / ct;

    r = k > 66 ? 1.292936 * pow(k - 60, -0.133205) : 1;
    g = k > 66 ? 1.129891 * pow(k - 60, -0.075515) : 0.390082 * log(k) - 0.631841;
    b = k < 66 ? 0.543207 * log(k - 10) - 1.196254 : 1;

    return Color(r, g, b);
}

Color Color::fromHS(double h, double s)
{
    double p = floor(h * 6), q = h * 6 - p, i = 1 - s, j = 1 - s * q, k = 1 - s * (1 - q);

    switch (static_cast <quint8> (p) % 6)
    {
        case 0:  return Color(1, k, i);
        case 1:  return Color(j, 1, i);
        case 2:  return Color(i, 1, k);
        case 3:  return Color(i, j, 1);
        case 4:  return Color(k, i, 1);
        case 5:  return Color(1, i, j);
    }

    return Color(0, 0, 0);
}

Color Color::fromXY(double x, double y)
{
    double z = 1 - x - y, p = 1 / y * x, q = 1 / y * z, r, g, b, max;

    r = correctGamma(p *  1.656492 - q * 0.255038 - 0.354851);
    g = correctGamma(p * -0.707196 + q * 0.036152 + 1.655397);
    b = correctGamma(p *  0.051713 + q * 1.011530 - 0.121364);

    max = r > g ? r > b ? r : b : g > b ? g : b;

    if (max > 1)
    {
        r /= max;
        g /= max;
        b /= max;
    }

    return Color(r, g, b);
}

void Color::toHS(double *h, double *s)
{
    double max = m_r > m_g ? m_r > m_b ? m_r : m_b : m_g > m_b ? m_g : m_b, min = m_r < m_g ? m_r < m_b ? m_r : m_b : m_g < m_b ? m_g : m_b, delta = max - min;

    if (!delta)
        *h = 0;
    else if (m_r == max)
        *h = ((m_g - m_b) + delta * (m_g < m_b ? 6 : 0)) / delta / 6;
    else if (m_g == max)
        *h = ((m_b - m_r) + delta * 2) / delta / 6;
    else if (m_b == max)
        *h = ((m_r - m_g) + delta * 4) / delta / 6;

    *s = max ? delta / max : 0;
}

void Color::toXY(double *x, double *y)
{
    double r = reverseGamma(m_r), g = reverseGamma(m_g), b = reverseGamma(m_b), z;

    *x = r * 0.664511 + g * 0.154324 + b * 0.162028;
    *y = r * 0.283881 + g * 0.668433 + b * 0.047685;
    z  = r * 0.000088 + g * 0.072310 + b * 0.986039 + *x + *y;

    *x /= z;
    *y /= z;
}

double Color::correctGamma(double value)
{
    return value <= 0.0031306684425006 ? value * 12.92 : pow(value, 1 / 2.4) * 1.055 - 0.055;
}

double Color::reverseGamma(double value)
{
    return value <= 0.0404482362771076 ? value / 12.92 : pow((value + 0.055) / 1.055, 2.4);
}
