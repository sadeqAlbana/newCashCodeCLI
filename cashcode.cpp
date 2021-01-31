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


    if(poll.status()==PollResponse::PowerUp){
        CCNetResponse reset=this->sendCommand(CCNet::deviceCommand::reset);
        qDebug()<<"reset: " << reset;

        poll=this->poll();
        if(poll.status()==PollResponse::Initilize){
            poll=this->poll();
            if(poll.status()==PollResponse::Initilize){
                poll=this->poll();
                if(poll.status()==PollResponse::Disabled){
                    GetStatusResponse getStatus=this->sendCommand(CCNet::deviceCommand::getStatus);
                    qDebug()<<getStatus.billTypes();
                    GetBillTableResponse getBillTable=this->sendCommand(CCNet::deviceCommand::getBillTable);
                    qDebug()<<getBillTable.billTable();

                    //should set security but leave it for now
                    CCNetResponse setSecurity=this->sendCommand(CCNet::deviceCommand::setSecurity,0,QByteArray::fromHex("000000"));
                    qDebug()<<"set security: "<<setSecurity;

                    IdentificationResponse identification=this->sendCommand(CCNet::deviceCommand::identification);
                    qDebug()<<identification.partNumber();
                }
            }
        }




    }




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


void CashCode::run(){
        bool has_error = false;
        int  balance=0;
        bool must_stop=false;
        int stackCounter=0;

        while (!must_stop && balance==0 && !has_error){
            CCNetResponse result = sendCommand(CCNet::deviceCommand::poll);
            qDebug()<<"res: "<<result;
            //qDebug()<<Q_FUNC_INFO << result << " ---------" <<"SIZE: " << result.size() << " -- " << (result[3]==0xff);
            if (result.error()!=CCNetResponse::NoError){
                //this->_ccnet->sendCommand(deviceCommand::NAK);
                has_error=true;
                qDebug()<<"Has error: " << result.error();

                continue;
            }
//            if (this->CheckErrors(result)){
//                has_error = true;
//                must_stop = true;
//                qDebug()<<"balance before Error: " << balance;
//                balance=0;
////                CashCodeError cc_error(this->_ccnet->m_LastError);
////                QString error_string(cc_error.getMessage().c_str());
////                KLog::log("error/cashcode",error_string);
////                this->_ccnet->sendCommand(deviceCommand::NAK);
//                continue;
//            }
//            if (result[3] == '\x00'){
//                //this->_ccnet->sendCommand(deviceCommand::POLL);
//                continue;
//            }
//            if (result[3] == '\x14'){
//                //this->_ccnet->sendCommand(deviceCommand::ACK);
//                continue;
//            }

            if (result.z1() != 0x14){
//                if (result[3] == 0x15){
//                    // Accepting
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
//                else if (result[3] == 0x1c){
//                    // ESCROW Position
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
                 if (result.z1() == 0x80){
                    //this->_ccnet->sendCommand(deviceCommand::ACK);
                    qDebug()<<"Stack sent !";
                    qDebug()<<"rr:" << sendCommand(CCNet::deviceCommand::stackBill);
                }
//                else if (result[3] == 0x17){
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
                else if (result.z1() == 0x81){
                    //Bill Stacked
                    qDebug()<<"Bill Stacked:";
                    stackCounter++;
                    qDebug()<<"stack counter: " << stackCounter;
                    //if(stackCounter==3){
                        //this->_ccnet->sendCommand(deviceCommand::ACK);
                        balance = this->channels[result.z2()];
                        must_stop=true;
                        qDebug()<<"balance:" << balance;
                    //}
                }
//                else if (result[3] == '\x18'){
//                    // Returning
//                    //this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
//                else if (result[3] == 0x82){
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
            }
            QThread::msleep(200); //was 50000 -> 100000 //recommended is 200000
        }
}

void CashCode::enableBillTypes()
{

    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("FFFFFFFFFFFF"));
    qDebug()<<"enable bill types z1: " << res.z1();

    PollResponse poll=sendCommand(CCNet::deviceCommand::poll);

    qDebug()<<poll;
    qDebug()<<poll.status();
    qDebug()<<poll.failureReason();
}

PollResponse CashCode::poll()
{
    PollResponse res = sendCommand(CCNet::deviceCommand::poll);

    QThread::msleep(400);

    return res;


}
