#include <QCoreApplication>
#include <QDir>
#include <QLockFile>
#include <signal.h>
#include "controller.h"

static void signalHandler(int)
{
    QCoreApplication::quit();
}

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    QLockFile lock(QString("%1%2%3.lock").arg(QDir::tempPath(), QDir::separator(), application.applicationName()));

    if (application.arguments().value(1) == "-v")
    {
        printf("%s %s\n", application.applicationName().toUtf8().constData(), SERVICE_VERSION);
        return EXIT_SUCCESS;
    }

    if (lock.tryLock(1000))
    {
        int result = EXIT_RESTART;

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        while (result == EXIT_RESTART)
        {
            Controller controller;
            QObject::connect(&application, &QCoreApplication::aboutToQuit, &controller, &Controller::quit);
            result = application.exec();
        }

        return result;
    }

    printf("Service already running\n");
    return EXIT_FAILURE;
}
