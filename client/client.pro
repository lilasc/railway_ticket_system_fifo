CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0


HEADERS += \
    core.h \
    ../protocal.h

SOURCES += \
    interface.c \
    client.c \
    callbacks.c
