#include <QCoreApplication>
#include "device.h"
#include <QTimer>
#include <QObject>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer time;
    device Dev("COM6");
    QObject::connect(&time, SIGNAL(timeout()), &Dev, SLOT(work()));
    time.setInterval(Dev.settings->value("Device/timer").toInt()*1000);
    Dev.read(Dev.makeWriteData(1),1);
    Dev.read(Dev.makeWriteData(2),2);
    time.start();

    return a.exec();
}
