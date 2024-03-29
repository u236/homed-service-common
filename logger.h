#ifndef LOGGER_H
#define LOGGER_H

#define logInfo             qInfo()
#define logWarning          qWarning()

#include <QDebug>

void setLogEnabled(bool value);
void setLogFile(const QString &value);
void logger(QtMsgType type, const QMessageLogContext &context, const QString &message);

#endif
