#ifndef PARSER_H
#define PARSER_H

#include <math.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class Expression
{

public:

    Expression(QString text);
    inline double result(void) { return m_result; }

private:

    enum class Type
    {
        Empty,
        Number,
        OpenBracket,
        CloseBracket,
        Add,
        Subtract,
        Multiply,
        Divide,
        Remainder,
        Pow,
        Round,
        Ceil,
        Floor,
        Sqrt,
        Log,
        Ln,
        Exp,
        Cosd,
        Cosr,
        Coshd,
        Coshr,
        ACosd,
        ACosr,
        Sind,
        Sinr,
        Sinhd,
        Sinhr,
        ASind,
        ASinr,
        Tgd,
        Tgr,
        Tghd,
        Tghr,
        ATgr,
        ATgd,
        Random
    };

    struct Item
    {
        Type type;
        QString value;
    };

    inline double radian(double value) { return value / 180 * M_PI; }

    double m_result;
    QVector <Item> m_items;

    Type itemType(const QString &v);
    int itemPriority(Type type);

    void calculate(void);

};

class Parser
{

public:

    static QVariant jsonValue(const QJsonObject &json, const QString &path);
    static QVariant stringValue(const QString &string);

};

#endif
