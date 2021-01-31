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
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);





    CashCode cashcode;




    if(!cashcode.open()){
        qDebug()<<"could not open !";
        return 0;
    }
    qDebug()<<"opened !";


    qDebug()<<"sleep ended";



        cashcode.powerup();





    QThread::sleep(5);




    cashcode.close();


    return a.exec();
}
