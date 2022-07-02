#include <QCoreApplication>
#include <QDateTime>
#include <QFileInfo>
#include <iostream>
#include "logger.h"

void logger(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context)

    QString service = QCoreApplication::applicationName().split("-").last(), data = QString("%1 (%2) %3 %4").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz"), type == QtInfoMsg ? "inf" : "wrn", service.append(':').leftJustified(10), message);
    QFile file(LOG_FILE_PATH);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << data << Qt::endl;
        file.close();
    }

    std::cout << data.toStdString() << std::endl;
}
