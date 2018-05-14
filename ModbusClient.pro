#-------------------------------------------------
#
# Project created by QtCreator 2018-03-01T10:47:03
#
#-------------------------------------------------

QT       += core gui

TARGET = ModbusClient
TEMPLATE = app


SOURCES += main.cpp\
        modbusclient.cpp \
    audiofile/header.cpp \
    audiofile/getlinkingmachine.cpp \
    audiofile/endecode.cpp \
    usernumtotype.cpp \
    testfivesecond.cpp \
    modbuscomply.cpp \
    mixervolume.cpp \
    cmodbustcpslave.cpp \
    alsainit.cpp \
    audiofile/heartbeatreturn.cpp

HEADERS  += modbusclient.h \
    audiofile/header.h \
    audiofile/getlinkingmachine.h \
    audiofile/endecode.h \
    usernumtotype.h \
    testfivesecond.h \
    modbuscomply.h \
    mixervolume.h \
    cmodbustcpslave.h \
    alsainit.h \
    audiofile/heartbeatreturn.h

FORMS    += modbusclient.ui

INCLUDEPATH += -I /opt/alsa-lib/include/ \
               -I /opt/Speex/speex1.2beta2/include \
               -I /opt/libmodbus/include/ \
               -I ./

LIBS    += -L/opt/alsa-lib/lib/ -lasound

LIBS    += -L/opt/libmodbus/lib/ -lmodbus

LIBS    += -L/opt/Speex/speex1.2beta2/lib/ -lspeex
