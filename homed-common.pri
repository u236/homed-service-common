HEADERS += \
    $$PWD/gpio.h \
    $$PWD/gpio.h \
    $$PWD/homed.h \
    $$PWD/logger.h

SOURCES += \
    $$PWD/gpio.cpp \
    $$PWD/homed.cpp \
    $$PWD/logger.cpp \
    $$PWD/main.cpp

QT -= gui
QT += mqtt

CONFIG += c++17 console
INCLUDEPATH += ../homed-common/

target.path = /usr/local/bin
INSTALLS += target
