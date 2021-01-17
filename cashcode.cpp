#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
CashCode::CashCode(QObject *parent) : QObject(parent)
{
    serial.setPortName("/dev/ttyUSB0");
}

bool CashCode::open()
{
   bool success= serial.open(QIODevice::ReadWrite);

   return success;
}

bool CashCode::close()
{
    serial.close();
}

QByteArray CashCode::sendCommand(QByteArray command)
{
    serial.clear();
    QByteArray result;

    if(!serial.write(command))
        return QByteArray();

    if(!serial.waitForBytesWritten()) //official is 10ms
        return QByteArray("w bytes written");

    //QThread::usleep(10000); //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

    if(command.at(3)=='\x00'){
        return "ACK";
    }
    if(command.at(3)=='\xff'){
        return "NAK";
    }

    // start reading response

    QList<char> res;

    int timeout= command.at(3)=='\x30' ? 2000 : 10;


    if(serial.waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication
        result.append(serial.readAll());
        while(serial.waitForReadyRead(5)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            result.append(serial.readAll());
            //qDebug()<<"result: " << result;
        }
        //QThread::usleep(20000); //20ms
        return result;
    }
    else{

        //QThread::usleep(20000); //20ms
        return QByteArray("W ready read");
    }
}
