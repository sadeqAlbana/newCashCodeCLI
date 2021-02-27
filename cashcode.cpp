#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
#include <QCoreApplication>
#include "pollresponse.h"
#include "getstatusresponse.h"
#include "getbilltableresponse.h"
#include "identificationresponse.h"
#include "ccnetexception.h"
CashCode::CashCode(QString port, QObject *parent) : QObject(parent)
{
    //qDebug()<<"cashcode cons thread: " << this->thread();

    serial.setPortName(port);
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
    //qDebug()<<"sent thread: " << this->thread();

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

bool CashCode::powerup()
{
    PollResponse poll=this->poll();

    if(poll.status()==PollResponse::Disabled){
        qDebug()<<"validator is already ready to work !";
        return true;
    }

    if(poll.status()==PollResponse::PowerUp || poll.status()==PollResponse::PowerUpWithBillinValidator || poll.status()==PollResponse::PowerUpWithBillinStacker){
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
                    return true;
                }
                else{
                    qDebug()<<"waiting ...";
                    //exit with some error code ?
                    //return false;
                }
            }


        } //initilize

    } //powerup

    return true;
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



void CashCode::disableBillTypes()
{
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("000000"));
    qDebug()<<"disable bill types: " << res.z1();
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

int CashCode::operate(bool &mustStop)
{
    //qDebug()<<"operate thread: " << this->thread();

    bool finished=false;
    int stackedBill=-1;
    bool billStacked=false;

    while (!finished && !mustStop) {
        PollResponse poll=this->poll();

        PollResponse::Status status=poll.status();
        //qDebug()<<"status: " << status;
        switch (status) {

        case PollResponse::Cheated: //happens when you pull the bill manually from the stacker side
            throw CCNetException(CCNetException::CheatedError);

        case PollResponse::ValidatorJammed: //happens when you block the validator from acceptance path
            throw CCNetException(CCNetException::ValidatorJammedError);

        case  PollResponse::DropCassetteFull:
            throw CCNetException(CCNetException::DropCassetteFullError);
            ;
        case PollResponse::DropCassetteJammed: //happens when you hold the bill from stacker side when the validator is trying to return it
            throw CCNetException(CCNetException::DropCassetteJammedError);

        case PollResponse::DropCassetteOutOfPosition:
            throw  CCNetException(CCNetException::DropCassetteOutOfPositionError);

        case PollResponse::Pause: //deadlock state
            throw CCNetException(CCNetException::PausedError);


        case PollResponse::EscrowPosition:
        {
            int bill=poll.billType();
            qDebug()<<bill;
            log(status,bill);
            sendCommand(CCNet::deviceCommand::stackBill);
        }
            break;

        case PollResponse::Idling:{ //must return to idling after stacking the bill
            if(billStacked){
                finished=true;
               // mustStop=true;

            }
        }
            break;



        case PollResponse::BillStacked:{
            stackedBill=poll.billType();
            billStacked=true;
            log(status,stackedBill);
            //finished=true;
            qDebug()<<"stacked bill: " << stackedBill;
        }
            break;
        case PollResponse::BillReturned:
        {
            log(status,poll.billType());
        }
            break;

        }

    }
    qDebug()<<"returned !";
    return stackedBill;
}

void CashCode::log(PollResponse::Status status, int billType)
{
    QString statusStr=toString(status);

}

void CashCode::log(PollResponse::Status status, PollResponse::GenericFailureReason)
{

}

void CashCode::log(PollResponse::Status status, PollResponse::RejectReason)
{

}

void CashCode::log(PollResponse::Status status)
{

}

void CashCode::enableBillTypes(int bill)
{
    std::vector<quint8> params = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    switch (bill) {
    case 1000:
        params = {0x80, 0x20, 0x08, 0x80, 0x20, 0x08};
        break;
    case 5000:
        params = {0x00, 0x00, 0x10, 0x00, 0x00, 0x10};
        break;
    case 6000:
        params = {0x00, 0x20, 0x18, 0x00, 0x20, 0x18};
        break;
    case 10000:
        params = {0x00, 0x80, 0x20, 0x00, 0x80, 0x20};
        break;
    case 15000:
        params = {0x00, 0x80, 0x30, 0x00, 0x80, 0x30};
        break;
    case 16000:
        params = {0x80 ,0xA0, 0x38, 0x80, 0xA0, 0x38};
        break;
    case 25000:
        params = {0x01, 0x00, 0x40 ,0x01, 0x00, 0x40};
        break;
    case 40000:
        params = {0x01, 0x80, 0x70, 0x01, 0x80, 0x70};
        break;
    case 41000:
        params = {0x81, 0xA0, 0x78, 0x81, 0xA0, 0x78};
        break;
    case 50000:
        params = {0x02, 0x00, 0x80, 0x02, 0x00, 0x80};
        break;
    case 65000:
        params = {0x03, 0x80, 0xF0, 0x03, 0x80, 0xF0};
        break;
    case 90000:
        params = {0x03, 0x80 ,0xF0, 0x03, 0x80, 0xF0};
        break;
    case 91000:
        params = {0x83, 0xA0, 0xF8, 0x83, 0xA0, 0xF8};
        break;
    default:
        params = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        break;
    }

    QByteArray data((char *)params.data(),params.size());

    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,data);
    qDebug()<<"enable bill types z1: " << res.z1();
    PollResponse poll=this->poll();

}


