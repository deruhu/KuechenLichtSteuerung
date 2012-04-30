#include <QtGui/QApplication>
#include "kuechenlichtcontrol.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KuechenLichtControl w;
    w.show();

    return a.exec();
}
