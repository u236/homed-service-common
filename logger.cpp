#include <QCoreApplication>
#include <QDateTime>
#include <QFileInfo>
#include <iostream>
#include "logger.h"

static bool enabled;
static QFile file;

void setLogEnabled(bool value)
{
    enabled = value;
}

void setLogFile(const QString &value)
{
    file.setFileName(value);
}

void logger(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    QString service = QCoreApplication::applicationName().split("-").last(), data = QString("%1 (%2) %3").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"), type == QtInfoMsg ? "inf" : "wrn", service.append(':').leftJustified(12));

    data.append(message.front().toUpper());
    data.append(message.mid(1));

    if (enabled && file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << data << Qt::endl;
        file.close();
    }

    std::cout << data.toStdString() << std::endl;
}
