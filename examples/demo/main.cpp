#include <QCoreApplication>
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
#include <QBitArray>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    CCNet::BillTable billTable;
    billTable.insert(0,{"IRQ",50});
    billTable.insert(1,{"IRQ",250});
    billTable.insert(2,{"IRQ",500});
    billTable.insert(3,{"IRQ",1000});
    billTable.insert(4,{"IRQ",5000});
    billTable.insert(5,{"IRQ",10000});
    billTable.insert(6,{"IRQ",25000});
    billTable.insert(7,{"IRQ",50000});

    CashCode cashcode("/dev/ttyUSB0",billTable);

    if(!cashcode.open()){
        qDebug()<<"could not open !";
        return 0;
    }
    qDebug()<<"opened !";


    qDebug()<<"sleep ended";



    try{

        cashcode.powerup();

        QThread::sleep(2);

        //old method
        std::vector<quint8> params = {0x00, 0x00, 0x08, 0x0, 0x00, 0x08};
        cashcode.enableBillTypes(params);


        //new method
        //QBitArray ba(24,false);
        //ba.setBit(2,true); //this would be later given by an index map
        //cashcode.enableBillTypes(ba);

        //GetBillTableResponse res=cashcode.sendCommand(CCNet::deviceCommand::getBillTable);
        //qDebug()<<res.billTable();
        //cashcode.requireBill(CCNet::Bill("IRQ",5000));
//        cashcode.enableBillTypes({0xff,0xff,0xff,0xff,0xff,0xff});

//        bool mustStop=false;
//        cashcode.operate(mustStop);
//        cashcode.disableBillTypes();


    }
    catch(CCNetException e){
        qDebug()<<"CCNet Error: " << e.type() << " " << e.callerMethod() << " " << e.additionalInfo();
    }


    cashcode.close();


    return a.exec();
}
