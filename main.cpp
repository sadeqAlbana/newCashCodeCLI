#include <QCoreApplication>
#include "serialport.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <iostream>
#include "cashcode.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);




    //std::vector<char> reset({ '\x02', '\x03', '\x06', '\x30', '\x41', '\xB3' });
    //std::vector<char> ack( { '\x02', '\x03', '\x06', '\x00', '\xC2', '\x82' });
    //std::vector<char> poll( { '\x02', '\x03', '\x06', '\x33', '\xDA', '\x81' });
    std::vector<char> getStatus({ '\x02', '\x03', '\x06', '\x31', '\xC8', '\xA2' });

    QByteArray reset=QByteArray::fromHex("0203063041b3");
    QByteArray ack=QByteArray::fromHex("02030600c282");
    QByteArray poll=QByteArray::fromHex("02030633da81");
    QByteArray nak=QByteArray::fromHex("020306ffba8d");



    CashCode cashcode;
    if(!cashcode.open()){
        qDebug()<<"could not open !";
        return 0;
    }
    qDebug()<<"opened !";

    QThread::sleep(1);

    qDebug()<<"sleep ended";


//    for (int i=0;i<20; i++) {
//        qDebug()<<"poll: " << cashcode.sendCommand(poll);
//        cashcode.sendCommand(ack);
//        QThread::msleep(500);
//    }

    qDebug()<<"Reset: "<< cashcode.sendCommand(reset);
    cashcode.sendCommand(ack);

    QThread::sleep(2);
    qDebug()<<"sleep 2 ended";



    QTimer t1;
    QTimer t2;
    t1.setInterval(200);
    //t2.setInterval(200);
    QObject::connect(&t1,&QTimer::timeout,[&t1,&t2,&cashcode](){
        //t1.stop();
        QByteArray poll=QByteArray::fromHex("02030633da81");
        QByteArray ack=QByteArray::fromHex("02030600c282");
        qDebug()<<"poll: " << cashcode.sendCommand(poll);
        cashcode.sendCommand(ack);
        //t2.start();
    });

//    QObject::connect(&t2,&QTimer::timeout,[&t1,&t2,&cashcode](){
//        t2.stop();
//        QByteArray ack=QByteArray::fromHex("02030600c282");
//        qDebug()<<"ack:" << cashcode.sendCommand(ack).toHex(' ');
//        t1.start();

//    });

    t1.start();


//    cashcode.close();
//    qDebug()<<"closed !";
//    exit(0);


    return a.exec();
}
