#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <stdint.h>
#include <QJsonObject>
#include <QDateTime>
#include <QJsonDocument>
#include <QSettings>
#include <QJsonArray>

class device : public QObject
{
    Q_OBJECT
public:
    explicit device(QString portName, QObject *parent = 0);
    ~device();
    QByteArray makeWriteData(int type_);
    read (QByteArray writeData,int type_);
    QSettings* settings;

signals:

public slots:
    work();

private:
    int stat = 0;
    QSerialPort port;
    int check(QByteArray crc);
    send(double val, int status,int type_);
};

#endif // DEVICE_H
