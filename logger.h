#ifndef LOGGER_H
#define LOGGER_H

#define LOG_FILE_PATH       "/var/log/homed.log"

#define logInfo             qInfo()
#define logWarning          qWarning()

#include <QDebug>

void logger(QtMsgType type, const QMessageLogContext &context, const QString &message);

#endif
