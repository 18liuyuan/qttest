#-------------------------------------------------
#
# Project created by QtCreator 2017-03-31T15:24:36
#
#-------------------------------------------------

QT       -=  gui

TARGET = ThingsSDK
TEMPLATE = lib

DEFINES += THINGSSDK_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/connection.cpp \
    src/cwlib.cpp \
    src/tenv.cpp \
    src/texpr.cpp \
    src/things.cpp \
    src/tobject.cpp \
    src/tparser.cpp \
    src/tutils.cpp \
    src/tvarex.cpp \
    src/vsync.cpp \
    src/base64.c \
    src/conn_win_socket.cpp \
    thingssdk.cpp \
    thingsloopthread.cpp

HEADERS +=\
    thingssdk_global.h \
    src/include/base64.h \
    src/include/common.h \
    src/include/conn_win_socket.h \
    src/include/connection.h \
    src/include/cwlib.h \
    src/include/tenv.h \
    src/include/texpr.h \
    src/include/things.h \
    src/include/tobject.h \
    src/include/tparser.h \
    src/include/tutils.h \
    src/include/tvarex.h \
    src/include/vsync.h \
    thingssdk.h \
    thingsloopthread.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DEFINES += WIN32
LIBS += -lWs2_32
