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
#include "utils.h"
#include "identificationresponse.h"
#include "getbilltableresponse.h"
#include "ccnetexception.h"
#include "pollresponse.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    CashCode cashcode;

    //PollResponse::Status status=PollResponse::Busy;

//    QByteArray array((char *)&status,1);
//    //array.detach();
//    qDebug()<<&status;
//    qDebug()<<(void*)array.constData();
//    return 0;


    if(!cashcode.open()){
        qDebug()<<"could not open !";
        return 0;
    }
    qDebug()<<"opened !";


    qDebug()<<"sleep ended";



    try{

        cashcode.powerup();

        QThread::sleep(2);
        cashcode.enableBillTypes();
        cashcode.operate();
        cashcode.disableBillTypes();

    }
    catch(CCNetException e){
        qDebug()<<"CCNet Error: " << e.type() << " " << e.callerMethod() << " " << e.additionalInfo();
    }


    cashcode.close();


    return a.exec();
}
