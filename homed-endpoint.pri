HEADERS += \
    $$PWD/endpoint.h \
    $$PWD/expose.h

SOURCES += \
    $$PWD/endpoint.cpp \
    $$PWD/expose.cpp

expose.files = $$PWD/deploy/data/usr/share/homed-common/expose.json
expose.path = /usr/share/homed-common

INSTALLS += expose
