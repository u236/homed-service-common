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
QMAKE_POST_LINK = $(STRIP) $(TARGET)

CONFIG += c++17 console ltcg exceptions_off rtti_off
INCLUDEPATH += ../homed-common/

target.path = /usr/local/bin
INSTALLS += target
