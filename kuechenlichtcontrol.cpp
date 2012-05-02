#include "kuechenlichtcontrol.h"
#include "ui_kuechenlichtcontrol.h"
#include <sstream>
#include <Qt>
#include <QColor>
#include <QColorDialog>
#include <QMessageBox>
#include <ace/SOCK_Dgram.h>
#include <ace/INET_Addr.h>
#include "/home/deruhu/git/ethersex/protocols/kuechenlichtprotokoll/udpkuechenlicht.h"


KuechenLichtControl::KuechenLichtControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KuechenLichtControl)
{
    ui->setupUi(this);
    ACE_INET_Addr   kRspAddr (KUECHENLICHT_UDP_RSP_PORT);
    ACE_SOCK_Dgram_Bcast  kBCSocket(kRspAddr);

    findModules();

}

KuechenLichtControl::~KuechenLichtControl()
{
    delete ui;
}

void KuechenLichtControl::on_sendButton_clicked()
{
    QString IPString;
    IPString=ui->lineEditIP->text();
    ACE_INET_Addr remote((u_short)KUECHENLICHT_UDP_PORT,IPString.toStdString().c_str());
    ACE_INET_Addr local (KUECHENLICHT_UDP_PORT);
    ACE_SOCK_Dgram sendsocket(local);
    kuechenLichtLEDStatus kLEDStatus;


    ledrgb.rotString=ui->lineEditRot->text();
    ledrgb.gruenString=ui->lineEditGruen->text();
    ledrgb.blauString=ui->lineEditBlau->text();
    ledFarbe.setRed(ledrgb.rotString.toInt());
    ledFarbe.setGreen(ledrgb.gruenString.toInt());
    ledFarbe.setBlue(ledrgb.blauString.toInt());
    kLEDStatus.rot=(unsigned char)ledFarbe.red();
    kLEDStatus.gruen=(unsigned char)ledFarbe.green();
    kLEDStatus.blau=(unsigned char)ledFarbe.blue();

    while()
    {

        size_t sent_data_length = sendsocket.send (&kLEDStatus, sizeof (kuechenLichtLEDStatus), remote);

        if(sent_data_length==-1)
        {   QMessageBox::critical(this, tr("Error"),
                                  tr("Konnte die Daten nicht senden!"));
            return;
        }
    }
    sendsocket.close();
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

void KuechenLichtControl::findModules(void)
{

}
