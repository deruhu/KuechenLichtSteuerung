#-------------------------------------------------
#
# Project created by QtCreator 2012-04-30T12:09:11
#
#-------------------------------------------------

QT       += core gui

TARGET = KuechenLichtSteuerung
TEMPLATE = app


SOURCES += main.cpp\
        kuechenlichtcontrol.cpp

HEADERS  += \
    ../ethersex/protocols/kuechenlichtprotokoll/udpkuechenlicht.h \
    kuechenlichtcontrol.h

FORMS    += kuechenlichtcontrol.ui
