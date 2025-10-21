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
        Comma,
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
        Exp,
        Min,
        Max,
        Random
    };

    struct Item
    {
        Type type;
        QString value;
    };

    double m_result;
    QVector <Item> m_items;

    Type itemType(const QString &v);
    int itemPriority(Type type);

    void calculate(void);

};

class Parser
{

public:

    static QString formatValue(const QString &string);
    static QVariant jsonValue(const QByteArray &data, const QString &path);
    static QString urlValue(const QByteArray &string, const QString &key);
    static QString xmlValue(const QByteArray &string, const QString &key);
    static QVariant stringValue(const QString &string);

};

#endif
