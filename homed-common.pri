HEADERS += \
    $$PWD/homed.h \
    $$PWD/logger.h

SOURCES += \
    $$PWD/homed.cpp \
    $$PWD/logger.cpp \
    $$PWD/main.cpp

DISTFILES += \
    $$PWD/deploy/data/usr/share/homed-common/expose.json

QT -= gui
QT += mqtt

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os
QMAKE_POST_LINK = $(STRIP) $(TARGET)

CONFIG += c++17 console exceptions_off ltcg object_parallel_to_source rtti_off
INCLUDEPATH += ../homed-common/

target.path = /usr/bin
INSTALLS += target
