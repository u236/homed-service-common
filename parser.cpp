#include <QRandomGenerator>
#include <QStack>
#include "parser.h"

Expression::Expression(QString string) : m_result(NAN)
{
    QRegExp error("([^0-9a-z\\+\\-\\*\\/\\%\\^\\(\\)\\.\\ ])"), number("([0-9]+\\.?[0-9]*)"), negative(QString("(^\\-|[\\+\\-\\*\\/\\^\\(]-)").append(number.pattern())), expression(number.pattern().append("|([()])|([\\+\\-\\*\\/\\%\\^])|(round|ceil|floor|sqrt|log|ln|exp|cosd|cosr|coshd|coshr|acosd|acosr|sind|sinr|sinhd|sinhr|asind|asinr|tgd|tgr|tghd|tghr|atgd|atgr|random)"));
    QVector <Item> items;
    QStack <Item> operationStack;
    QStack <int> priorityStack;
    int position = 0, offset = 0;

    string.remove(0x0A);
    string.remove(0x0D);
    string.remove(0x20);

    if (string.isEmpty() || error.indexIn(string) != -1 || string.count("(") != string.count(")") || string.contains("()"))
        return;

    while ((position = negative.indexIn(string, position)) != -1)
    {
        QString value = negative.cap();
        string.replace(position, value.length(), position ? QString("%1(0%2)").arg(value.at(0), value.mid(1)) : QString("(0%1)").arg(value));
        position += negative.matchedLength();
    }

    position = 0;

    while ((position = expression.indexIn(string, position)) != -1)
    {
        QString value = expression.cap();

        if (number.indexIn(value) != -1)
            items.append({Type::Number, value});
        else
            items.append({itemType(value), value});

        position += expression.matchedLength();
    }

    for (int i = 0; i < items.count(); i++)
    {
        const Item &item = items.at(i);
        int priority;

        switch (item.type)
        {
            case Type::Empty: return;
            case Type::Number: m_items.append(item); continue;
            case Type::OpenBracket: offset += 10; continue;
            case Type::CloseBracket: offset -= 10; continue;
            default: break;
        }

        priority = itemPriority(item.type) + offset;

        if (!operationStack.isEmpty() && (priority < priorityStack.top() || (priority == priorityStack.top() && ((item.type != Type::Add && item.type != Type::Multiply) || item.value != operationStack.top().value))))
        {
            for (int i = 0, count = operationStack.count(); i < count; i++)
            {
                if (priorityStack.top() < priority)
                    break;

                m_items.append(operationStack.pop());
                priorityStack.pop();
            }
        }

        operationStack.push(item);
        priorityStack.push(priority);
    }

    for (int i = 0, count = operationStack.count(); i < count; i++)
        m_items.append(operationStack.pop());

    if (m_items.count() == 1 && m_items.value(0).type == Type::Number)
    {
        m_result = m_items.value(0).value.toDouble();
        return;
    }

    calculate();
}

Expression::Type Expression::itemType(const QString &value)
{
    if (value == "(")      return Type::OpenBracket;
    if (value == ")")      return Type::CloseBracket;
    if (value == "+")      return Type::Add;
    if (value == "-")      return Type::Subtract;
    if (value == "*")      return Type::Multiply;
    if (value == "/")      return Type::Divide;
    if (value == "%")      return Type::Remainder;
    if (value == "^")      return Type::Pow;
    if (value == "round")  return Type::Round;
    if (value == "ceil")   return Type::Ceil;
    if (value == "floor")  return Type::Floor;
    if (value == "sqrt")   return Type::Sqrt;
    if (value == "log")    return Type::Log;
    if (value == "ln")     return Type::Ln;
    if (value == "exp")    return Type::Exp;
    if (value == "cosd")   return Type::Cosd;
    if (value == "cosr")   return Type::Cosr;
    if (value == "coshd")  return Type::Coshd;
    if (value == "coshr")  return Type::Coshr;
    if (value == "acosd")  return Type::ACosd;
    if (value == "acosr")  return Type::ACosr;
    if (value == "sind")   return Type::Sind;
    if (value == "sinr")   return Type::Sinr;
    if (value == "sinhd")  return Type::Sinhd;
    if (value == "sinhr")  return Type::Sinhr;
    if (value == "asind")  return Type::ASind;
    if (value == "asinr")  return Type::ASinr;
    if (value == "tgd")    return Type::Tgd;
    if (value == "tgr")    return Type::Tgr;
    if (value == "tghd")   return Type::Tghd;
    if (value == "tghr")   return Type::Tghr;
    if (value == "atgd")   return Type::ATgd;
    if (value == "atgr")   return Type::ATgr;
    if (value == "random") return Type::Random;

    return Type::Empty;
}

int Expression::itemPriority(Type type)
{
    switch (type)
    {
        case Type::Add:       return 1;
        case Type::Subtract:  return 1;
        case Type::Multiply:  return 2;
        case Type::Divide:    return 2;
        case Type::Remainder: return 2;
        case Type::Pow:       return 3;
        default:              return 4;
    }
}

void Expression::calculate(void)
{
    QVector <Type> items;
    QVector <int> index;
    QVector <double> result;
    int count = 0, i = 0, a = 0, b = 0, c = 1;

    for (int i = 0; i < m_items.count(); i++)
    {
        const Item &item = m_items.at(i);

        if (item.type != Type::Number)
            index.append(i);

        switch (item.type)
        {
            case Type::Add:
            case Type::Subtract:
            case Type::Multiply:
            case Type::Divide:
            case Type::Remainder:
            case Type::Pow:
                count++;
                break;

            default:
                break;
        }

        items.append(item.type);
        result.append(item.type != Type::Number ? NAN : item.value.toDouble());
    }

    if (index.isEmpty() || count != m_items.count() - index.count() - 1)
        return;

    while (i < index.count())
    {
        if (items.at(c) != Type::Empty && items.at(c) != Type::Number)
        {
            switch (items.at(index.at(i++)))
            {
                case Type::Add:       result.replace(a, result.at(a) + result.at(b)); items.replace(b, Type::Empty);      break;
                case Type::Subtract:  result.replace(a, result.at(a) - result.at(b)); items.replace(b, Type::Empty);      break;
                case Type::Multiply:  result.replace(a, result.at(a) * result.at(b)); items.replace(b, Type::Empty);      break;
                case Type::Divide:    result.replace(a, result.at(a) / result.at(b)); items.replace(b, Type::Empty);      break;
                case Type::Remainder: result.replace(a, fmod(result.at(a), result.at(b))); items.replace(b, Type::Empty); break;
                case Type::Pow:       result.replace(a, pow(result.at(a), result.at(b))); items.replace(b, Type::Empty);  break;
                case Type::Round:     result.replace(b, round(result.at(b)));                                             break;
                case Type::Ceil:      result.replace(b, ceil(result.at(b)));                                              break;
                case Type::Floor:     result.replace(b, floor(result.at(b)));                                             break;
                case Type::Sqrt:      result.replace(b, sqrt(result.at(b)));                                              break;
                case Type::Log:       result.replace(b, log10(result.at(b)));                                             break;
                case Type::Ln:        result.replace(b, log(result.at(b)));                                               break;
                case Type::Exp:       result.replace(b, exp(result.at(b)));                                               break;
                case Type::Cosd:      result.replace(b, cos(radian(result.at(b))));                                       break;
                case Type::Cosr:      result.replace(b, cos(result.at(b)));                                               break;
                case Type::Coshd:     result.replace(b, cosh(radian(result.at(b))));                                      break;
                case Type::Coshr:     result.replace(b, cosh(result.at(b)));                                              break;
                case Type::ACosd:     result.replace(b, acos(radian(result.at(b))));                                      break;
                case Type::ACosr:     result.replace(b, acos(result.at(b)));                                              break;
                case Type::Sind:      result.replace(b, sin(radian(result.at(b))));                                       break;
                case Type::Sinr:      result.replace(b, sin(result.at(b)));                                               break;
                case Type::Sinhd:     result.replace(b, sinh(radian(result.at(b))));                                      break;
                case Type::Sinhr:     result.replace(b, sinh(result.at(b)));                                              break;
                case Type::ASind:     result.replace(b, asin(radian(result.at(b))));                                      break;
                case Type::ASinr:     result.replace(b, asin(result.at(b)));                                              break;
                case Type::Tgd:       result.replace(b, tan(radian(result.at(b))));                                       break;
                case Type::Tgr:       result.replace(b, tan(result.at(b)));                                               break;
                case Type::Tghd:      result.replace(b, tanh(radian(result.at(b))));                                      break;
                case Type::Tghr:      result.replace(b, tanh(result.at(b)));                                              break;
                case Type::ATgd:      result.replace(b, atan(radian(result.at(b))));                                      break;
                case Type::ATgr:      result.replace(b, atan(result.at(b)));                                              break;
                case Type::Random:    result.replace(b, round(QRandomGenerator::global()->bounded(result.at(b))));        break;
                default:                                                                                                  break;
            }

            items.replace(c, Type::Empty);
        }

        b = c++;

        while (items.at(b) == Type::Empty)
            b--;

        a = b > 0 ? b - 1 : 0;

        while (items.at(a) == Type::Empty)
            a--;
    }

    m_result = result.at(0);
}

QString Parser::formatValue(const QString &string)
{
    QRegExp regexp("\\((.*)\\)");

    if (regexp.indexIn(string) != -1)
    {
        QList <QString> actionList = {"fromHex", "toHex", "time"};

        switch (actionList.indexOf(string.mid(0, string.indexOf(0x28))))
        {
            case 0: // fromHex
            {
                QByteArray data = QByteArray::fromHex(regexp.cap(1).toUtf8());
                QList <QString> list;

                for (int i = 0; i < data.length(); i++)
                    list.append(QString::number(static_cast <quint8> (data.at(i))));

                return list.join(',');
            }

            case 1: // toHex
            {
                QList <QString> list = regexp.cap(1).split(',');
                QByteArray data;

                for (int i = 0; i < list.count(); i++)
                    data.append(static_cast <char> (list.at(i).toInt()));

                return QString(data.toHex()).toUpper();
            }

            case 2: // time
            {
                bool check;
                QList <QString> list = regexp.cap(1).split('|');
                qint64 value = list.value(1).toLongLong(&check);
                QDateTime dateTime = check ? QDateTime::fromSecsSinceEpoch(value) : QDateTime::currentDateTime();
                return list.value(0).isEmpty() ? QString::number(dateTime.currentSecsSinceEpoch()) : dateTime.toString(list.value(0).trimmed());
            }

            default:
                break;
        }
    }

    return string;
}

QVariant Parser::jsonValue(const QByteArray &data, const QString &path)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    QList <QString> list = path.split('.');
    QJsonValue value;

    for (int i = 0; i < list.count(); i++)
    {
        QString key = list.at(i);
        int index = -1;

        if (key.endsWith(']'))
        {
            int position = key.indexOf('[');
            index = key.mid(position + 1, key.length() - position - 2).toInt();
            key = key.mid(0, position);
        }

        if (!key.isEmpty())
            value = document.object().value(key);

        if (index >= 0)
            value = value.isArray() ? value.toArray().at(index) : document.array().at(index);

        if (i < list.count() - 1)
            document = value.isArray() ? QJsonDocument(value.toArray()) : QJsonDocument(value.toObject());
    }

    return value.toVariant();
}


QString Parser::urlValue(const QByteArray &string, const QString &key)
{
    QList <QByteArray> list = string.split('&');

    for (int i = 0; i < list.count(); i++)
    {
        QList <QByteArray> item = list.at(i).split('=');

        if (item.value(0) != key)
            continue;

        return QUrl::fromPercentEncoding(item.value(1));
    }

    return QString();
}

QVariant Parser::stringValue(const QString &string)
{
    bool check;
    double value = string.toDouble(&check);

    if (check)
        return value;

    if (string == "true" || string == "false")
        return string == "true" ? true : false;

    return string.isEmpty() ? QVariant() : string;
}
