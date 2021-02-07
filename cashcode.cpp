#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
#include <QCoreApplication>
#include "pollresponse.h"
#include "getstatusresponse.h"
#include "getbilltableresponse.h"
#include "identificationresponse.h"
#include "ccnetexception.h"
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

    if(!serial.clear())
        throw CCNetException(CCNetException::SerialClearError);


    if(!serial.write(message))
        throw CCNetException(CCNetException::SerialWriteTimeout);

    if(!serial.waitForBytesWritten()) //official is 10ms
        throw CCNetException(CCNetException::SerialReadTimeout);


    unsigned long utimeout= 10000;
    QThread::usleep(utimeout);

    // start reading response

    int timeout = 3000;

    QByteArray result;
    if(serial.waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication
        result.append(serial.readAll());
        if((quint8)result[CCNet::SyncOffset]!= 0x02){
            qDebug()<<"result : " << result.toHex(' ');
            sendNAK();
            throw CCNetException(CCNetException::SyncError);
        }

        while(!messageComplete(result)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            if(!serial.waitForReadyRead(20)){
                sendNAK();
                throw CCNetException(CCNetException::IncompleteResponseTimout);
            }
            result.append(serial.readAll());
        }

        //result is now complete, check crc16

        bool crcValid=validateResponse(result);
        if(!crcValid){
            sendNAK();
            throw CCNetException(CCNetException::CRCError);
        }

        sendACK();
        return CCNetResponse(result);
    }
    else{
        sendNAK();
        throw CCNetException(CCNetException::SerialReadTimeout);
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

    if(poll.status()==PollResponse::Disabled){
        qDebug()<<"validator is already ready to work !";
        return 0;
    }

//    if(poll.status()==PollResponse::PowerUpWithBillinValidator){
//        qDebug()<<"Bill was found inside the validator....returning bill !";

//        return 0;
//    }

    if(poll.status()==PollResponse::PowerUp || poll.status()==PollResponse::PowerUpWithBillinValidator){
        CCNetResponse reset=this->sendCommand(CCNet::deviceCommand::reset);
        //qDebug()<<"reset: " << reset;

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
                            qDebug()<<"waiting ...";
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
        QByteArray length=QByteArray((char *) &packageLength,2);
        std::reverse(length.begin(),length.end()); // LNG always follows MSB first
        data.prepend(length);
    }else{
        message.append(QByteArray((char *)&packageLength,1)); //LNG
    }

    message.append((quint8)cmd);
    if(subCmd>0) //append subCMD if present
        message.append((quint8)subCmd);


    message.append(data);

    quint16 crc=Utils::crc16(message);
    QByteArray crcData=QByteArray((char *)&crc,2);
//    qDebug()<<"Crc data:" <<crc << " ---- " << crcData;

    message.append(crcData);

    return message;
}


void CashCode::enableBillTypes()
{
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("FFFFFFFFFFFF"));
    qDebug()<<"enable bill types z1: " << res.z1();
    PollResponse poll=this->poll();
}

void CashCode::disableBillTypes()
{
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("000000"));
    qDebug()<<"enable bill types z1: " << res.z1();
    PollResponse poll=this->poll();
}

PollResponse CashCode::poll()
{
    PollResponse res = sendCommand(CCNet::deviceCommand::poll);
    if(res.status()==PollResponse::GenericFailure){
        qDebug()<<res.status() << " "  << res.failureReason();
        throw CCNetException(CCNetException::GenericFailure,Q_FUNC_INFO,QJsonObject{{"response",QString(res.message().toHex(' '))},
                                                                                    {"failureReason",toString(res.failureReason())}});
    }
    else if(res.status()==PollResponse::Rejecting){
        qDebug()<<res.status() << " " << res.rejectReason();
    }
    else if(res.status()==PollResponse::Busy){
        qDebug()<<res.status() << " for: " << (res.z2()*100);
        QThread::msleep((res.z2()*100));
    }
    else{
        qDebug()<<res.status();
    }
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

        case PollResponse::Pause:
        {
            //TODO
        }
            break;

        case PollResponse::Busy: //deadlock state
        {
            //TODO
        }
            break;

        case PollResponse::BillReturned:
        {
            logReturnedBill(poll.billType());
        }
            break;

        case PollResponse::Cheated: //happens when you pull the bill manually from the stacker side
        {
            //TODO
        }
            break;

        case PollResponse::ValidatorJammed: //happens when you block the validator from acceptance path
        {
            //TODO
        }
            break;

        case PollResponse::DropCassetteFull:
        {
            //TODO
        }
            break;
        case PollResponse::DropCassetteJammed: //happens when you hold the bill from stacker side when the validator is trying to return it
        {
            //TODO
        }
            break;

        case PollResponse::DropCassetteOutOfPosition:
        {
            //TODO
        }
            break;


        case PollResponse::Accepting :
            //qDebug()<<"accepting";
            break;
        case PollResponse::EscrowPosition:
        {
            int bill=poll.billType();
            qDebug()<<bill;

            //sendCommand(CCNet::deviceCommand::stackBill);
            //sendCommand(CCNet::deviceCommand::returnBill);
        }

            break;
        case PollResponse::BillStacked:
            stackedBill=poll.billType();
            finished=true;
            break;
        }

    }
    qDebug()<<"stacked bill: " << stackedBill;
}

void CashCode::logReturnedBill(int bill)
{

}
