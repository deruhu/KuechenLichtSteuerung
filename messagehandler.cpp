#include "messagehandler.h"
#include <QThread>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <udpkuechenlicht.h>
#include <map>
#include "global.h"

MessageHandler::MessageHandler()
{

}


bool MessageHandler::handleMessage()
{
    ACE_INET_Addr remote;
    char buff[1500]={0};
    int flags=1;
    size_t bufflen=sizeof(buff);

    int i=0;

    mutexSendSock.lock();
    ssize_t recv_cnt=pSendSocket->recv(buff,bufflen,remote,flags,&timeout);
    mutexSendSock.unlock();

    kuechenLichtHeader* pParseHeader;
    while((recv_cnt-i)>sizeof(kuechenLichtHeader))
    {
        pParseHeader=(kuechenLichtHeader*)&buff[i];
        i+=sizeof(kuechenLichtHeader);

        if((pParseHeader->magic==KUECHENLICHT_MAGIC)&&((recv_cnt-i)>=pParseHeader->messageLength))
        {
            switch(pParseHeader)
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
    return erfolg;
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
    if(!(mLichterMap.count(pRespond->kHeader.modulAddress)))
    {

        mLichterMap.insert(std::pair<std::string,kuechenLicht_rsp_set>(pIstSender->get_host_addr(),aNeuesLicht));
        mutexLichterMap.unlock();
        emit signalNewModule();
    }
    else
        mutexLichterMap.unlock();
    kuechenLicht_cmd_gotcha commandGotcha;
    commandGotcha.gotcha=true;
    commandGotcha.kHeader=pRespond->kHeader;
    commandGotcha.kHeader.messageID=KUECHENLICHT_ID_CMD_GOTCHA;
    commandGotcha.kHeader.messageLength=sizeof(kuechenLicht_cmd_gotcha);

    mutexSendSock.lock();
    size_t sent_data_length = pSendSocket->send(&commandGotcha, commandGotcha.kHeader.messageLength, *pIstSender);
    mutexSendSock.unlock();

    if(sent_data_length==-1)
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("Konnte Gotcha nicht senden!"));
        return;
    }

}

void MessageHandler::handleSetResponse(kuechenLicht_rsp_set* paKuechenLichtRsp,ACE_INET_Addr* pIstSender)
{
    mutexLichterMap.lock();
    mLichtermap[pIstSender->get_host_addr()]=paKuechenLichtRsp;
    mutexLichterMap.unlock();

    emit signalStatusUpdate();
}

