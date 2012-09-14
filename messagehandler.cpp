#include "messagehandler.h"
#include <ace/OS.h>
#include <QThread>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>
#include <ace/Time_Value.h>
#include <ace/Log_Msg.h>
#include <udpkuechenlicht.h>
#include <map>
#include "global.h"
#include <QMessageBox>
#include <QQueue>
#include <iostream>

MessageHandler::MessageHandler() : QObject(),
    kBCAddr (KUECHENLICHT_UDP_BC_PORT),
    kSendAddr (KUECHENLICHT_UDP_CMD_PORT),

    sendtimeout (0,2000)

{
    pBCSocket = new ACE_SOCK_Dgram_Bcast(kBCAddr);
    pSendSocket = new ACE_SOCK_Dgram(kSendAddr);
}


void MessageHandler::handleMessage()
{
    ACE_INET_Addr remote;
    char buff[1500]={0};
    int flags=1;
    size_t bufflen=sizeof(buff);

    int i=0;

    ssize_t recv_cnt=pSendSocket->recv(buff,bufflen,remote,flags,&sendtimeout);

    kuechenLichtHeader* pParseHeader;
    while((recv_cnt-i)>sizeof(kuechenLichtHeader))
    {
        pParseHeader=(kuechenLichtHeader*)&buff[i];
        i+=sizeof(kuechenLichtHeader);

        if((pParseHeader->magic==KUECHENLICHT_MAGIC)&&((recv_cnt-i)>=pParseHeader->messageLength))
        {
            switch(pParseHeader->messageID)
            {
            case KUECHENLICHT_ID_RSP_SET:
                handleSetResponse((kuechenLicht_rsp_set*)&buff[i],&remote);
                break;

            case KUECHENLICHT_ID_RSP_CAYF:
                handleCAYFResponse((kuechenLicht_rsp_cayf*)&buff[i],&remote);
                break;

            default:
                break;
            }
        }
    }

return;

}

void MessageHandler::handleCAYFResponse(kuechenLicht_rsp_cayf* pRespond, ACE_INET_Addr* pIstSender)
{
    kuechenLicht_rsp_set aNeuesLicht;

    aNeuesLicht.kHeader=pRespond->kHeader;
    aNeuesLicht.kHeader.messageID=KUECHENLICHT_ID_RSP_SET;
    aNeuesLicht.kHeader.messageLength=sizeof(kuechenLicht_rsp_set);
    aNeuesLicht.kLEDStatus=pRespond->kLEDStatus;
    aNeuesLicht.an=pRespond->an;

    mutexLichterMap.lock();
    if(!(mLichterMap.count(pIstSender->get_host_addr())))
    {
        mLichterMap.insert(std::pair<std::string,kuechenLicht_rsp_set>(pIstSender->get_host_addr(),aNeuesLicht));
        mutexLichterMap.unlock();
        emit signalNewModule(pIstSender->get_host_addr());
    }
    else
        mutexLichterMap.unlock();
    kuechenLicht_cmd_gotcha commandGotcha;
    commandGotcha.gotcha=true;
    commandGotcha.kHeader=pRespond->kHeader;
    commandGotcha.kHeader.messageID=KUECHENLICHT_ID_CMD_GOTCHA;
    commandGotcha.kHeader.messageLength=sizeof(kuechenLicht_cmd_gotcha);

    mutexSendQueue.lock();
    size_t sent_data_length = pSendSocket->send(&commandGotcha, commandGotcha.kHeader.messageLength, *pIstSender);
    mutexSendQueue.unlock();

    if(sent_data_length==-1)
    {/*
        QMessageBox::critical(this, tr("Error"),
                              tr("Konnte Gotcha nicht senden!"));*/
        return;
    }

    else
    {
        emit signalNewModule(pIstSender->get_host_addr());
    }
}

void MessageHandler::handleSetResponse(kuechenLicht_rsp_set* pKuechenLichtRsp,ACE_INET_Addr* pIstSender)
{
    mutexLichterMap.lock();
    mLichterMap[pIstSender->get_host_addr()]=* pKuechenLichtRsp;
    mutexLichterMap.unlock();

    emit signalStatusUpdate();
}

void MessageHandler::sendCommand()
{
    cmdMessageContainer sendBox;
    mutexSendQueue.lock();
    while(!commandMessageQueue.isEmpty())
    {

        sendBox=commandMessageQueue.dequeue();

        if(pSendSocket->send(&(sendBox.message[0]),sendBox.size,sendBox.modulAddr)==-1)
            std::cout<<"Senden fehlgeschlagen"<<std::endl;
    }

    mutexSendQueue.unlock();
}

void MessageHandler::sendBCMessage()
{
    bcMessageContainer sendBCBox;
    mutexBCQueue.lock();
    while(!broadcastMessageQueue.isEmpty())
    {

        sendBCBox=broadcastMessageQueue.dequeue();
        pBCSocket->send(&(sendBCBox.message[0]),sendBCBox.size,KUECHENLICHT_UDP_BC_PORT);
    }
    mutexBCQueue.unlock();
}
