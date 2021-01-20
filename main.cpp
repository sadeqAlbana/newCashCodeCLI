#include <QCoreApplication>
#include "serialport.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <iostream>
#include "cashcode.h"
#include "cashcodecommand.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);




    //std::vector<char> reset({ '\x02', '\x03', '\x06', '\x30', '\x41', '\xB3' });
    //std::vector<char> ack( { '\x02', '\x03', '\x06', '\x00', '\xC2', '\x82' });
    //std::vector<char> poll( { '\x02', '\x03', '\x06', '\x33', '\xDA', '\x81' });
    std::vector<char> getStatus({ '\x02', '\x03', '\x06', '\x31', '\xC8', '\xA2' });

//    QByteArray reset=QByteArray::fromHex("0203063041b3");
//    QByteArray ack=QByteArray::fromHex("02030600c282");
//    QByteArray poll=QByteArray::fromHex("02030633da81");
//    QByteArray nak=QByteArray::fromHex("020306ffba8d");



    CashCode cashcode;
//    qDebug()<<cashcode.createMessage(CashCode::deviceCommand::enableBillTypes,0,QByteArray::fromHex("83A0F883A0F8")).toHex(' ');
//    qDebug()<<"crc16 val: " << cashcode.crc16(QByteArray::fromHex("02030c3483a0f883a0f8"));
//    qDebug()<<"GetCRC val2: " << cashcode.GetCRC16(QByteArray::fromHex("02030c3483a0f883a0f8"));

//    return 0;



    if(!cashcode.open()){
        qDebug()<<"could not open !";
        return 0;
    }
    qDebug()<<"opened !";

    //QThread::sleep(1);

    qDebug()<<"sleep ended";


//    for (int i=0;i<20; i++) {
//        qDebug()<<"poll: " << cashcode.sendCommand(poll);
//        cashcode.sendCommand(ack);
//        QThread::msleep(500);
//    }

    //qDebug()<<"Reset: "<< cashcode.sendCommand(CashCode::deviceCommand::reset);
    cashcode.powerup();
    QThread::sleep(4);
    qDebug()<<"sleep 2 ended";



    QTimer t1;
    QTimer t2;
    t1.setInterval(200);
    //t2.setInterval(200);
    QObject::connect(&t1,&QTimer::timeout,[&t1,&t2,&cashcode](){
        //t1.stop();
        QByteArray poll=QByteArray::fromHex("02030633da81");
        QByteArray ack=QByteArray::fromHex("02030600c282");
        qDebug()<<"poll: " << cashcode.sendCommand(CashCode::deviceCommand::poll);
        //cashcode.sendCommand(CashCode::deviceCommand::ac);
        //t2.start();
    });

//    QObject::connect(&t2,&QTimer::timeout,[&t1,&t2,&cashcode](){
//        t2.stop();
//        QByteArray ack=QByteArray::fromHex("02030600c282");
//        qDebug()<<"ack:" << cashcode.sendCommand(ack).toHex(' ');
//        t1.start();

//    });

    //t1.start();

    QByteArray t;
    t.append('\x80');
    t.append('\x20');
    t.append('\x08');
    t.append('\x80');
    t.append('\x20');
    t.append('\x08');
    //std::reverse(t.begin(),t.end());

    //qDebug()<<"enable bill types escr: "<< cashcode.sendCommand(CashCode::deviceCommand::enableBillTypes,0,QByteArray::fromHex("FFFFFFFFFFFF"));


    //qDebug()<<"enable bill types escr: "<< cashcode.sendCommand(CashCode::deviceCommand::enableBillTypes,0,QByteArray::fromHex("000000000000"));

    qDebug()<<"enable bill types: "<< cashcode.sendCommand(CashCode::deviceCommand::enableBillTypes,0,t);
    QThread::msleep(10);

    cashcode.run();


    cashcode.close();
//    qDebug()<<"closed !";
//    exit(0);


    return a.exec();
}
