HEADERS += \
    $$PWD/homed.h \
    $$PWD/logger.h

SOURCES += \
    $$PWD/homed.cpp \
    $$PWD/logger.cpp \
    $$PWD/main.cpp

QT -= gui
QT += mqtt

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os

CONFIG += c++17 console
INCLUDEPATH += ../homed-common/

target.path = /usr/local/bin
INSTALLS += target
