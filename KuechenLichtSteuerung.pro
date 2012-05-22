#-------------------------------------------------
#
# Project created by QtCreator 2012-04-30T12:09:11
#
#-------------------------------------------------

QT       += core gui

TARGET = KuechenLichtSteuerung
TEMPLATE = app

LIBS    +=../../ACE_wrappers/lib/libACE.so \
          ../../ACE_wrappers/ace/libACE.so.6.0.6

INCLUDEPATH += ../../ACE_wrappers/ace/ \
            ../../ACE_wrappers/

INCLUDEPATH += ../ethersex/protocols/kuechenlichtprotokoll/

SOURCES += main.cpp\
        kuechenlichtcontrol.cpp \
    messagehandler.cpp

HEADERS  += \
    ../ethersex/protocols/kuechenlichtprotokoll/udpkuechenlicht.h \
    kuechenlichtcontrol.h \
    ../../ACE_wrappers/ace/SOCK_Dgram.h \
    ../../ACE_wrappers/ace/INET_Addr.h \
    messagehandler.h \
    global.h

FORMS    += kuechenlichtcontrol.ui
