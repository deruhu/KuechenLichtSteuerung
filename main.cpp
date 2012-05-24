#include <QtGui/QApplication>
#include "kuechenlichtcontrol.h"
#include <QColorDialog>
#include <QMutex>
#include <udpkuechenlicht.h>
#include <QThread>
#include <QObject>
#include <messagehandler.h>
#include <QTimer>
#include <QQueue>
#include <ace/INET_Addr.h>

std::map<std::string, kuechenLicht_rsp_set>    mLichterMap;

typedef struct{
   char             message[500];
   size_t           size;
}bcMessageContainer;

typedef struct{
   char             message[500];
   ACE_INET_Addr    modulAddr;
   size_t           size;
}cmdMessageContainer;

QQueue<cmdMessageContainer> commandMessageQueue;

QQueue<bcMessageContainer> broadcastMessageQueue;

QMutex mutexSendQueue,mutexBCQueue,mutexLichterMap;

volatile bool alive=true;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KuechenLichtControl* w = new KuechenLichtControl;

    QThread* pthread=new QThread();
    QThread* timerthread=new QThread();

    MessageHandler *pmessagehandler = new MessageHandler;

    QTimer *timerMessagePolling=new QTimer(0);

    timerMessagePolling->setInterval(500);

    timerMessagePolling->moveToThread(timerthread);

    pmessagehandler->moveToThread(pthread);



    QObject::connect(timerMessagePolling, SIGNAL(timeout()), pmessagehandler, SLOT(handleMessage()));

    QObject::connect(pmessagehandler, SIGNAL(signalNewModule(std::string)),w,SLOT(newModuleSlot(std::string)));

    QObject::connect(pmessagehandler,SIGNAL(signalStatusUpdate()),w,SLOT(changedStatusSlot()));

    QObject::connect(w,SIGNAL(sendCmdMessages()),pmessagehandler,SLOT(sendCommand()));

    QObject::connect(w,SIGNAL(sendBCMessages()),pmessagehandler,SLOT(sendBCMessage()));

    pthread->start();

    w->setup();
    w->show();



    return a.exec(); //Benutzerinteraktion startet
}
