#include "kuechenlichtcontrol.h"
#include "ui_kuechenlichtcontrol.h"
#include <sstream>
#include <QColor>
#include <QColorDialog>
#include <ace/SOCK_Dgram.h>
#include <ace/INET_Addr.h>


KuechenLichtControl::KuechenLichtControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KuechenLichtControl)
{
    ui->setupUi(this);
}

KuechenLichtControl::~KuechenLichtControl()
{
    delete ui;
}

void KuechenLichtControl::on_sendButton_clicked()
{
    static const u_short portKuechenLicht = 4898;
    ACE_INET_Addr remote;
    ACE_SOCK_Dgram sendsocket;
    ACE_INET_Addr local ((u_short) 0);

    sendsocket.open(local);
    udp.send();
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
