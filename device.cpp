#include "device.h"
#include "network.h"

device::device(QString portName, QObject *parent) : QObject(parent)
{
        port.setPortName(portName);
        port.open(QIODevice::ReadWrite);
        settings = new QSettings("configs.ini", QSettings::IniFormat);
}

device::read(QByteArray writeData,int type_)
{
    QByteArray readData;
    port.write(writeData);
    if(port.waitForReadyRead(5000))
    {
        readData = port.readAll();
        while (port.waitForReadyRead(5000)) {
            readData+= port.readAll();
        }
    }
    qDebug() << readData.toHex();
    // Getting value from high (valHi,hi) byte and low (valLo,lo) byte
    QByteArray valHi;
    QByteArray valLo;
    valHi.append(readData[3]);
    valLo.append(readData[4]);
    int hi = valHi.toHex().toInt(0,16);
    int lo = valLo.toHex().toInt(0,16);
    double val = double((hi*256+lo))/100;
    int answRes = check(readData); //Проверка CRC, возвращение результата о данных с прибора (1 - не отвечает, 2 - частично отвечает, 3 - полные данные)
    send(val, answRes, type_);
}

device::send(double val,  int status,int type_)
{
    network net;
    QDateTime dateTime;
    QJsonObject sendData;
    QJsonObject sendStatus;
    //Сообщение с данными
    sendData["value_numeric"] = val;
    if (type_==1)
    {
        sendData["meas_type_uid"] = settings->value("Device/meas_type_uid_t").toString();
    }
    else if(type_ == 2)
    {
        sendData["meas_type_uid"] = settings->value("Device/meas_type_uid_rh").toString();
    }
    sendData["device_uid"] = settings->value("Device/device_uid").toString();
    sendData["datetime"] = dateTime.currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    sendData["measurement_uid"] = dateTime.currentMSecsSinceEpoch();
    //Сообщение со статусом
    qDebug() << "Value: " << val << "Status: " << status;
    QJsonDocument docData (sendData);
    if (status == 3) net.doUpload(settings->value("Device/urlData").toString(), docData.toJson());
    stat += status;
    if (stat == 2)
    {
        sendStatus["device_uid"] = settings->value("Device/device_uid").toString();
        sendStatus["device_status_uid"] = QString::number(1);
        QJsonDocument docStatus (sendStatus);
        net.doUpload(settings->value("Device/urlStatus").toString(), docStatus.toJson());
        stat = 0;
    }
    else if (stat == 4)
    {
        sendStatus["device_uid"] = settings->value("Device/device_uid").toString();
        sendStatus["device_status_uid"] = QString::number(2);
        QJsonDocument docStatus (sendStatus);
        net.doUpload(settings->value("Device/urlStatus").toString(), docStatus.toJson());
        stat = 0;
    }
    else if(stat == 6)
    {
        sendStatus["device_uid"] = settings->value("Device/device_uid").toString();
        sendStatus["device_status_uid"] = QString::number(3);
        QJsonDocument docStatus (sendStatus);
        net.doUpload(settings->value("Device/urlStatus").toString(), docStatus.toJson());
        stat = 0;
    }

}

int device::check(QByteArray buf)
{
    QByteArray tempData  = buf;
    tempData.chop(2);
    QByteArray valHi;
    QByteArray valLo;
    valHi.append(buf[6]);
    valLo.append(buf[5]);
    int a1 = valHi.toHex().toInt(0,16)*256 + valLo.toHex().toInt(0,16);

    int len = tempData.length();
      uint crc = 0xFFFF;

      for (int pos = 0; pos < len; pos++)
      {
        crc ^= (uint)tempData[pos];       // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) {    // Loop over each bit
          if ((crc & 0x0001) != 0) {      // If the LSB is set
            crc >>= 1;                    // Shift right and XOR 0xA001
            crc ^= 0xA001;
          }
          else                            // Else LSB is not set
            crc >>= 1;                    // Just shift right
        }
      }
      qDebug() << a1 << " " << crc;
    if (crc == a1)  return 3;
    else return 1;
}

QByteArray device::makeWriteData(int type_)
{
    QByteArray com = QByteArray("\x03",1);      // Command
    QByteArray size = QByteArray("\x00\x01",2); //Size to read
    QByteArray T = QByteArray("\x01\x02",2);    //Temperature address
    QByteArray Rh = QByteArray("\x01\x03",2);   //RH address
    QByteArray dt;
    // Adding elements
    int devAddr = settings->value("Device/deviceAddress").toInt();
    dt.append(devAddr);
    dt.append(com);
    if (type_==1)
    {
        dt.append(T);
    }
    else if(type_ == 2)
    {
        dt.append(Rh);
    }
    dt.append(size);

    int len = dt.length();
      uint crc = 0xFFFF;

      for (int pos = 0; pos < len; pos++)
      {
        crc ^= (uint)dt[pos];          // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) {    // Loop over each bit
          if ((crc & 0x0001) != 0) {      // If the LSB is set
            crc >>= 1;                    // Shift right and XOR 0xA001
            crc ^= 0xA001;
          }
          else                            // Else LSB is not set
            crc >>= 1;                    // Just shift right
        }
      }
      //Adding CRC
      QByteArray result;
      QDataStream ds(&result, QIODevice::WriteOnly);
      ds << crc;
      dt.append(result[3]);
      dt.append(result[2]);
      return dt;
}

device::work()
{
    read(makeWriteData(1),1);
    read(makeWriteData(2),2);
}

device::~device()
{
    port.close();
}
