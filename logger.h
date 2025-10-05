#ifndef LOGGER_H
#define LOGGER_H

#define logDebug(debug)     if (debug) qDebug()
#define logInfo             qInfo()
#define logWarning          qWarning()

#include <QDebug>

void setLogEnabled(bool value);
void setLogTimestams(bool value);
void setLogFile(const QString &value);
void logger(QtMsgType type, const QMessageLogContext &context, const QString &message);

#endif
