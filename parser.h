#ifndef PARSER_H
#define PARSER_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class Parser
{

public:

    static QVariant jsonValue(const QJsonObject &json, const QString &path);
    static QVariant stringValue(const QString &string);

};

#endif
