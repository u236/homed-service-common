#include <QCoreApplication>
#include <QDateTime>
#include <QFileInfo>
#include <iostream>
#include "logger.h"

static bool enabled, timestamps;
static QFile file;

static QString typeString(QtMsgType type)
{
    switch (type)
    {
        case QtDebugMsg: return "dbg";
        case QtWarningMsg: return "wrn";
        default: return "inf";
    }
}

void setLogEnabled(bool value)
{
    enabled = value;
}

void setLogTimestams(bool value)
{
    timestamps = value;
}

void setLogFile(const QString &value)
{
    file.setFileName(value);
}

void logger(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    QString service = QCoreApplication::applicationName().split("-").last(), timestamp = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"), data = QString("(%1) %2").arg(typeString(type), service.append(':').leftJustified(12));

    data.append(message.front().toUpper());
    data.append(message.midRef(1));

    if (enabled && file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << (timestamps ? QString("%1 %2").arg(timestamp, data) : data) << Qt::endl;
        file.close();
    }

    std::cout << QString("%1 %2").arg(timestamp, data).toStdString() << std::endl;
}
