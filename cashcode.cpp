#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
#include <QCoreApplication>
CashCode::CashCode(QObject *parent) : QObject(parent)
{
    QString arg=QCoreApplication::arguments().value(1);
    if(arg.isEmpty())
        arg="/dev/ttyUSB0";

    serial.setPortName(arg);
}

bool CashCode::open()
{
    bool success= serial.open(QIODevice::ReadWrite);

    return success;
}

void CashCode::close()
{
    serial.close();
}

QByteArray CashCode::sendCommand(QByteArray command)
{
    serial.clear();


    if(!serial.write(command))
        return QByteArray();

    if(!serial.waitForBytesWritten()) //official is 10ms
        return QByteArray("w bytes written");

    unsigned long utimeout= command.at(3)=='\x30' ? 2000000 : 10000;


    QThread::usleep(utimeout); //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

    if(command.at(3)=='\x00'){
        //QThread::usleep(20000);
        return "ACK";
    }
    if(command.at(3)=='\xff'){
        //QThread::usleep(20000);
        return "NAK";
    }

    // start reading response


    int timeout= command.at(3)=='\x30' ? 2000 : 10;

    QByteArray result;
    if(serial.waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication
        result.append(serial.readAll());
        if(result[SyncOffset]!='\x02')
            return QByteArray("sync error");


        while(!messageComplete(result)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            if(!serial.waitForReadyRead(10))
                return QByteArray("timeout");
            result.append(serial.readAll());
        }

        //result is now complete, check crc16
        bool crcValid=validateResponse(result);
        qDebug()<<"crc validation: " << crcValid;


        //QThread::usleep(20000); //20ms
        return result;
    }
    else{

        //QThread::usleep(20000); //20ms
        return QByteArray("W ready read");
    }
}

QByteArray CashCode::sendCommand(const CashCode::deviceCommand &cmd, const quint8 &subCmd, const QByteArray &data)
{

    QByteArray message=createMessage(cmd,subCmd,data);

    serial.clear();


    if(!serial.write(message))
        return QByteArray();

    if(!serial.waitForBytesWritten()) //official is 10ms
        return QByteArray("w bytes written");

    unsigned long utimeout= message.at(3)=='\x30' ? 2000000 : 10000;


    QThread::usleep(utimeout); //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

//    if(message.at(3)=='\x00'){
//        //QThread::usleep(20000);
//        return "ACK";
//    }
//    if(message.at(3)=='\xff'){
//        //QThread::usleep(20000);
//        return "NAK";
//    }

    // start reading response


    int timeout= message.at(3)=='\x30' ? 2000 : 20;

    QByteArray result;
    if(serial.waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication
        result.append(serial.readAll());
        if(result[SyncOffset]!='\x02'){
            sendNAK();
            return QByteArray("sync error");
        }


        while(!messageComplete(result)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            if(!serial.waitForReadyRead(10)){
                sendNAK();
                return QByteArray("timeout");
            }
            result.append(serial.readAll());
        }

        //result is now complete, check crc16
        bool crcValid=validateResponse(result);
        qDebug()<<"crc validation: " << crcValid;


        //QThread::usleep(20000); //20ms
        sendACK();
        return result;
    }
    else{

        //QThread::usleep(20000); //20ms
        sendNAK();
        return QByteArray("W ready read");
    }


}

quint16 CashCode::crc16(const QByteArray data) const
{
    quint16 CRC;
    unsigned char j;
    CRC = 0;
    for(int i=0; i < data.size(); i++)
    {
        CRC ^= data[i];
        for(j=0; j < 8; j++)
        {
            if(CRC & 0x0001) {CRC >>= 1; CRC ^= POLYNOMIAL;}
            else CRC >>= 1;
        }
    }
    return CRC;
}

bool CashCode::messageComplete(const QByteArray &data) const
{
    if(data.size()>=3){
        int length=data[LNGoffset];
        if(length==data.length())
            return true;
    }
    return false;
}

bool CashCode::validateResponse(QByteArray data) const
{
    if(data.size()<3)
        return false;

    QByteArray crc=data.right(2);
    std::reverse(crc.begin(),crc.end());
    bool ok=false;
//    crc.append(data[data.size()-1]);
//    crc.append(data[data.size()-2]);

    quint16 messageCRC=crc.toHex().toUInt(&ok,16);
    if(!ok)
        return false;

    data.chop(2);
    return (messageCRC==crc16(data));

}

bool CashCode::sendACK()
{
    serial.clear();
    QByteArray ack=QByteArray::fromHex("02030600c282");
    serial.write(ack);
    return serial.waitForBytesWritten();
}

bool CashCode::sendNAK()
{
    serial.clear();
    QByteArray nak=QByteArray::fromHex("020306ffba8d");
    serial.write(nak);
    return serial.waitForBytesWritten();
}

QByteArray CashCode::createMessage(const CashCode::deviceCommand &cmd, const quint8 &subCmd, QByteArray data)
{
    QByteArray message;
    message.append(0x02); //sync
    message.append(0x03); //adr

    quint16 packageLength=6; //initial size sync + adr + lng + cmd + crc16 2 bytes

    if(subCmd>0)
        packageLength++;

    packageLength+=data.size();


    if(packageLength>250){
        message.append('\x00'); //set LNG to 0 then append package size to data block bytes 0 and 1
        packageLength+=2; //two length bytes
        QByteArray length=QByteArray::fromRawData((char *) &packageLength,2);
        std::reverse(length.begin(),length.end()); // LNG always follows MSB first
        data.prepend(length);
    }else{
        message.append(QByteArray::fromRawData((char *)&packageLength,1)); //LNG
    }

    message.append((quint8)cmd);
    if(subCmd) //append subCMD if present
        message.append((quint8)subCmd);


    message.append(data);

    quint16 crc=crc16(message);
    QByteArray crcData=QByteArray::fromRawData((char *)&crc,2);
    //std::reverse(crcData.begin(),crcData.end());
//    qDebug()<<"Crc data:" <<crc << " ---- " << crcData;

    message.append(crcData);
    qDebug()<<"message hex: " << message.toHex(' ');
    return message;
}
