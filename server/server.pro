CONFIG += link_pkgconfig
LIBS += -L/usr/lib/mysql -lmysqlclient

HEADERS += \
    app_mysql.h \
    ../protocal.h

SOURCES += \
    server.c \
    app_mysql.c
