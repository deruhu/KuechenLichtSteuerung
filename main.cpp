#include <QtGui/QApplication>
#include "kuechenlichtcontrol.h"
#include <QColorDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KuechenLichtControl w;
    w.setup();
    w.show();


    return a.exec();
}
