#ifndef COLOR_H
#define COLOR_H

#include <QtGlobal>

class Color
{

public:

    Color(double r, double g, double b) : m_r(r < 0 ? 0 : r < 1 ? r : 1), m_g(g < 0 ? 0 : g < 1 ? g : 1), m_b(b < 0 ? 0 : b < 1 ? b : 1) {}

    inline double r(void) { return m_r; }
    inline double g(void) { return m_g; }
    inline double b(void) { return m_b; }

    static Color fromCT(double ct);
    static Color fromHS(double h, double s);
    static Color fromXY(double x, double y);

    void toHS(double *h, double *s);
    void toXY(double *x, double *y);

private:

    double m_r, m_g, m_b;

    static double correctGamma(double value);
    static double reverseGamma(double value);

};

#endif
