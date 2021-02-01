#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
#include <QCoreApplication>
#include "pollresponse.h"
#include "getstatusresponse.h"
#include "getbilltableresponse.h"
#include "identificationresponse.h"
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



CCNetResponse CashCode::sendCommand(const CCNet::deviceCommand &cmd, const quint8 &subCmd, const QByteArray &data)
{

    QByteArray message=createMessage(cmd,subCmd,data);

    serial.clear();


    if(!serial.write(message))
        return CCNetResponse(QByteArray(),CCNetResponse::TimeoutError);

    if(!serial.waitForBytesWritten()) //official is 10ms
        return CCNetResponse(QByteArray(),CCNetResponse::TimeoutError);

    unsigned long utimeout= message.at(3)=='\x30' ? 2000000 : 10000;


    QThread::usleep(utimeout); //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

    // start reading response


    int timeout= message.at(3)=='\x30' ? 3000 : 3000;

    QByteArray result;
    if(serial.waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

        result.append(serial.readAll());
        if(result[CCNet::SyncOffset]!='\x02'){
            sendNAK();
            //qDebug()<<result;
            return CCNetResponse(result,CCNetResponse::TimeoutError);
        }


        while(!messageComplete(result)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            if(!serial.waitForReadyRead(10)){
                sendNAK();

                return CCNetResponse(result,CCNetResponse::TimeoutError);
            }
            result.append(serial.readAll());
        }

        //result is now complete, check crc16


        bool crcValid=validateResponse(result);
        if(!crcValid){
            sendNAK();
            return CCNetResponse(result,CCNetResponse::CRCError);
        }


        //QThread::usleep(20000); //20ms
        sendACK();
        //qDebug()<<result;
        return CCNetResponse(result,CCNetResponse::NoError);
    }
    else{

        //QThread::usleep(20000); //20ms
        sendNAK();
        //qDebug()<<result;
        return CCNetResponse(result,CCNetResponse::TimeoutError);
    }


}




bool CashCode::messageComplete(const QByteArray &data) const
{
    if(data.size()>=3){
        quint8 length=(unsigned char)data[CCNet::LNGoffset];  //this will not do if package is 255 bytes or larger
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


    quint16 messageCRC=crc.toHex().toUInt(&ok,16);
    if(!ok)
        return false;

    data.chop(2);

    return (messageCRC==Utils::crc16(data));

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

int CashCode::powerup()
{
    PollResponse poll=this->poll();
    //qDebug()<<poll.status();

    if(poll.status()==PollResponse::Disabled){
        qDebug()<<"validator is already ready to work !";
        return 0;
    }




    if(poll.status()==PollResponse::PowerUp){
        CCNetResponse reset=this->sendCommand(CCNet::deviceCommand::reset);
        qDebug()<<"reset: " << reset;

        poll=this->poll();
        if(poll.status()==PollResponse::Initilize){
                poll=this->poll();
                    GetStatusResponse getStatus=this->sendCommand(CCNet::deviceCommand::getStatus);
                    qDebug()<<getStatus.billTypes();
                    GetBillTableResponse getBillTable=this->sendCommand(CCNet::deviceCommand::getBillTable);
                    qDebug()<<getBillTable.billTable();

                    //should set security but leave it for now
                    CCNetResponse setSecurity=this->sendCommand(CCNet::deviceCommand::setSecurity,0,QByteArray::fromHex("000000"));
                    qDebug()<<"set security: "<<setSecurity;

                    IdentificationResponse identification=this->sendCommand(CCNet::deviceCommand::identification);
                    qDebug()<<identification.partNumber();



                    for(int i=0; i<50; i++){
                        poll=this->poll();
                        if(poll.status()==PollResponse::Disabled){
                         qDebug()<<"bill validator ready to work !";
                         return 0;
                        }
                        else{
                            //exit with some error code ?
                        }
                    }


                } //initilize

        } //powerup


}




QByteArray CashCode::createMessage(const CCNet::deviceCommand &cmd, const quint8 &subCmd, QByteArray data)
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
    if(subCmd>0) //append subCMD if present
        message.append((quint8)subCmd);


    message.append(data);

    quint16 crc=Utils::crc16(message);
    QByteArray crcData=QByteArray::fromRawData((char *)&crc,2);
    //std::reverse(crcData.begin(),crcData.end());
//    qDebug()<<"Crc data:" <<crc << " ---- " << crcData;

    message.append(crcData);
    //qDebug()<<"message hex: " << message.toHex();
    return message;
}


void CashCode::enableBillTypes()
{

    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("FFFFFFFFFFFF"));
    qDebug()<<"enable bill types z1: " << res.z1();

    PollResponse poll=sendCommand(CCNet::deviceCommand::poll);


}

void CashCode::disableBillTypes()
{
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("000000"));
    qDebug()<<"enable bill types z1: " << res.z1();

    PollResponse poll=sendCommand(CCNet::deviceCommand::poll);
}

PollResponse CashCode::poll()
{
    PollResponse res = sendCommand(CCNet::deviceCommand::poll);

    QThread::msleep(200);

    return res;

}

void CashCode::operate()
{
    bool finished=false;
    int stackedBill=-1;

    while (!finished) {
        PollResponse poll=this->poll();
        PollResponse::Status status=poll.status();
        //qDebug()<<"status: " << status;
        switch (status) {

        case PollResponse::Accepting :
            //qDebug()<<"accepting";
            break;
        case PollResponse::EscrowPosition:
            sendCommand(CCNet::deviceCommand::stackBill);
            break;
        case PollResponse::BillStacked:
            stackedBill=poll.stackedBill();
            finished=true;
            break;
        }

    }
    qDebug()<<"stacked bill: " << stackedBill;
}
