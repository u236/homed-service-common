#include <QCoreApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QMutexLocker>
#include <iostream>
#include "logger.h"

static bool enabled, timestamps;
static QFile file;
static QMutex mutex;
static QString service;

static QString typeString(QtMsgType type)
{
    switch (type)
    {
        case QtDebugMsg: return "dbg";
        case QtWarningMsg: return "wrn";
        default: return "inf";
    }
}

static QString formatMessage(QString message)
{
    message.front() = message.front().toUpper();
    return message;
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
    QMutexLocker lock(&mutex);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"), data;

    if (service.isEmpty())
        service = QCoreApplication::applicationName().split('-').last().append(':').leftJustified(11);

    data = QString("(%1) %2 %3").arg(typeString(type), service, formatMessage(message));

    if (enabled && file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << QString("%1 %2").arg(timestamp, data) << Qt::endl;
        file.close();
    }

    std::cout << (timestamps ? QString("%1 %2").arg(timestamp, data) : data).toStdString() << std::endl;
}
