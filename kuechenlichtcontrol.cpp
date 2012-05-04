#include "kuechenlichtcontrol.h"
#include "ui_kuechenlichtcontrol.h"
#include <sstream>
#include <Qt>
#include <QColor>
#include <QColorDialog>
#include <QMessageBox>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <udpkuechenlicht.h>
//#include "/home/deruhu/git/ethersex/protocols/kuechenlichtprotokoll/udpkuechenlicht.h"


KuechenLichtControl::KuechenLichtControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KuechenLichtControl)
{
    timeout(1);
    mBCAddressString("192.168.1.128");
    ui->setupUi(this);    
}

KuechenLichtControl::setup()
{
    ACE_INET_Addr   kBCAddr (KUECHENLICHT_UDP_BC_PORT);
    ACE_INET_Addr   kRspAddr (KUECHENLICHT_UDP_RSP_PORT);
    ACE_INET_Addr   kSendAddr (KUECHENLICHT_UDP_CMD_PORT);

    pBCSocket =new(ACE_SOCK_Dgram_Bcast(kCmdAddr));
    pRecSocket =new(ACE_SOCK_Dgram(kRspAddr));
    pSendSocket =new(ACE_SOCK_Dgram(kSendAddr));

    pRecSocket->open(kRspAddr);
    mBCAddress.set(KUECHENLICHT_UDP_BC_PORT,mBCAddressString.c_str());

    findModules();
}

KuechenLichtControl::~KuechenLichtControl()
{
    delete ui;
    delete pBCSocket;
    delete mBCAddress;
}

void KuechenLichtControl::on_sendButton_clicked()
{
    QString IPString;
    IPString=ui->lineEditIP->text();
    ACE_INET_Addr remote((u_short)KUECHENLICHT_UDP_SEND_PORT,IPString.toStdString().c_str());


    ledrgb.rotString=ui->lineEditRot->text();
    ledrgb.gruenString=ui->lineEditGruen->text();
    ledrgb.blauString=ui->lineEditBlau->text();
    ledFarbe.setRed(ledrgb.rotString.toInt());
    ledFarbe.setGreen(ledrgb.gruenString.toInt());
    ledFarbe.setBlue(ledrgb.blauString.toInt());
    kSenderLEDStatus.rot=(unsigned char)ledFarbe.red();
    kSenderLEDStatus.gruen=(unsigned char)ledFarbe.green();
    kSenderLEDStatus.blau=(unsigned char)ledFarbe.blue();

    kuechenLicht_cmd_set commandSet={
                            {	KUECHENLICHT_MAGIC,
                                remote.get_ip_address(),
                                0,
                                KUECHENLICHT_ID_CMD_SET,
                                (ACE_UINT16)sizeof(kuechenLicht_cmd_set)
                            },
                            {
                                kSenderLEDStatus.rot,
                                kSenderLEDStatus.gruen,
                                kSenderLEDStatus.blau
                            },
                                kSenderLEDStatus.rot ^  kSenderLEDStatus.gruen ^  kSenderLEDStatus.blau,
                                };

    int kDurchlaeufe=0;
    bool erfolg=false;
    int i=0;
    while((kDurchlaeufe<15) && (!erfolg))
    {

        size_t sent_data_length = pSendSocket->send(&commandSet, sizeof (kuechenLicht_cmd_set), remote);

        if(sent_data_length==-1)
        {
            QMessageBox::critical(this, tr("Error"),
                                  tr("Konnte die Daten nicht senden!"));
            return;
        }

           erfolg= handleMessage(&remote,kSenderLEDStatus.);
        }
    }
}

void KuechenLichtControl::on_selectColorButton_clicked()
{
    ledrgb.rotString=ui->lineEditRot->text();
    ledrgb.gruenString=ui->lineEditGruen->text();
    ledrgb.blauString=ui->lineEditBlau->text();
    ledFarbe.setRed(ledrgb.rotString.toInt());
    ledFarbe.setGreen(ledrgb.gruenString.toInt());
    ledFarbe.setBlue(ledrgb.blauString.toInt());
    ledFarbe=colorDialog.getColor(ledFarbe);
    ledrgb.rotString.setNum(ledFarbe.red(),10);
    ledrgb.gruenString.setNum(ledFarbe.green(),10);
    ledrgb.blauString.setNum(ledFarbe.blue(),10);
    ui->lineEditRot->setText(ledrgb.rotString);
    ui->lineEditGruen->setText(ledrgb.gruenString);
    ui->lineEditBlau->setText(ledrgb.blauString);
}

void KuechenLichtControl::on_findModulesPushButton_clicked()
{
    findModules();
}

bool KuechenLichtControl::handleMessage(ACE_INET_Addr* pSollSender,uint16_t sollNachricht)
{
    ACE_INET_Addr remote;
    char buff[1500]={0};
    int flags=1;
    size_t bufflen=sizeof(buff);

    int i=0;

    ssize_t recv_cnt=pRecSocket->recv(buff,bufflen,remote,flags,&timeout);

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
                erfolg=handleSetResponse((kuechenLicht_rsp_set*)&buff[i],pSollSender,pIstSender);
                break;

            default:
                break;
            }

        }
    }
return erfolg;
}
void KuechenLichtControl::findModules(void)
{
    kuechenLicht_cmd_come_all_ye_faithful commandCome={
                                        {KUECHENLICHT_MAGIC,
                                        mBCAddress.get_ip_address(),
                                        0,
                                        KUECHENLICHT_ID_CMD_CAYF,
                                        (uint16_t)sizeof(kuechenLicht_cmd_come_all_ye_faithful)
                                        },
                                        0
                                        };
    unsigned char i=0;

    while(i<15)
    {
    commandCome.kNumberofCalls=i;
    pBCSocket->send(&commandCome,sizeof(kuechenLicht_cmd_come_all_ye_faithful),mBCAddress);

    i++;

    //
    // mLichterMap.insert(std::pair<ACE_UINT32,kuechenLicht_rsp_set>(respond.kHeader.receiverAddress,respond));
    //
    }
}

bool KuechenLichtControl::handleSetResponse(kuechenLicht_rsp_set* pKuechenLichtRsp,ACE_INET_Addr* pSollSender,ACE_INET_Addr* pIstSender)
{
    if(!(pSollSender->get_ip_address()==pIstSender->get_ip_address()))
    {
        return false;
    }

    if(pKuechenLichtRsp->kLEDStatus==kSenderLEDStatus)
    {
        return true;
    }
}
