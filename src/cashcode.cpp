#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
#include <QCoreApplication>
#include "pollresponse.h"
#include "getstatusresponse.h"
#include "getbilltableresponse.h"
#include "identificationresponse.h"
#include "ccnetexception.h"
#include <QBitArray>

CashCode::CashCode(QString port, const CCNet::BillTable &billTable, QObject *parent) : QObject(parent),m_serial(new CSerialPort(this)),m_billTable(billTable)
{
    //qDebug()<<"cashcode cons thread: " << this->thread();

    m_serial->setPortName(port);
}

bool CashCode::open()
{
    bool success= m_serial->open(QIODevice::ReadWrite);

    return success;
}

void CashCode::close()
{
    m_serial->close();
}



CCNetResponse CashCode::sendCommand(const CCNet::deviceCommand &cmd, const quint8 &subCmd, const QByteArray &data)
{
    //qDebug()<<"sent thread: " << this->thread();

    QByteArray message=createMessage(cmd,subCmd,data);

    if(!m_serial->clear())
        throw CCNetException(CCNetException::SerialClearError);


    if(!m_serial->write(message))
        throw CCNetException(CCNetException::SerialWriteTimeout);

    if(!m_serial->waitForBytesWritten()) //official is 10ms
        throw CCNetException(CCNetException::SerialReadTimeout);


    unsigned long utimeout= 10000;
    QThread::usleep(utimeout);

    // start reading response

    int timeout = 3000;

    QByteArray result;
    if(m_serial->waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication
        result.append(m_serial->readAll());
        if((quint8)result[CCNet::SyncOffset]!= 0x02){
            qDebug()<<"result : " << result.toHex(' ');
            sendNAK();
            throw CCNetException(CCNetException::SyncError);
        }

        while(!messageComplete(result)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            if(!m_serial->waitForReadyRead(20)){
                sendNAK();
                throw CCNetException(CCNetException::IncompleteResponseTimout);
            }
            result.append(m_serial->readAll());
        }

        //result is now complete, check address and crc16

        //add address check later !

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
    m_serial->clear();
    QByteArray ack=QByteArray::fromHex("02030600c282");
    m_serial->write(ack);
    bool success = m_serial->waitForBytesWritten();
    this->thread()->msleep(20); //Tfree(min.)
    return success;
}

bool CashCode::sendNAK()
{
    m_serial->clear();
    QByteArray nak=QByteArray::fromHex("020306ffba8d");
    m_serial->write(nak);
    bool success= m_serial->waitForBytesWritten();

    this->thread()->msleep(20); //Tfree(min.)

    return success;
}

bool CashCode::powerup()
{
    PollResponse poll=this->poll();

    if(poll.status()==PollResponse::Disabled){
        qDebug()<<"validator is already ready to work !";
        return true;
    }
    CCNetResponse reset=this->sendCommand(CCNet::deviceCommand::reset);

    if(poll.status()==PollResponse::PowerUp || poll.status()==PollResponse::PowerUpWithBillinValidator || poll.status()==PollResponse::PowerUpWithBillinStacker){
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

QString CashCode::port()
{
    return m_serial->portName();
}

void CashCode::disableBillTypes()
{
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,QByteArray::fromHex("000000"));
    qDebug()<<"disable bill types res z1: " << res.z1();
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

CCNet::Bill CashCode::operate(bool &mustStop)
{
    //qDebug()<<"operate thread: " << this->thread();

    bool finished=false;
    CCNet::Bill stackedBill;
    _billStacked=false;

    while (!finished && !mustStop) {
        PollResponse poll=this->pollRedundant();

        PollResponse::Status status=poll.status();
        //qDebug()<<"status: " << status;
        switch (status) {

        case PollResponse::Accepting: emit billEntered(true); break;
        case PollResponse::Rejecting: emit billEntered(false); break;

//        case PollResponse::Cheated: //happens when you pull the bill manually from the stacker side
//            throw CCNetException(CCNetException::CheatedError);

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
            CCNet::Bill bill=billType(poll.billIndex());
            qDebug()<<bill;
            log(status,bill);
            sendCommand(CCNet::deviceCommand::stackBill);
        }
            break;

        case PollResponse::Idling:{ //must return to idling after stacking the bill
            if(billStacked()){
                finished=true;
               // mustStop=true;

            }
        }
            break;



        case PollResponse::BillStacked:{
            stackedBill=billType(poll.billIndex());
            _billStacked=true;
            log(status,stackedBill);
            //finished=true;
            qDebug()<<"stacked bill: " << stackedBill;
        }
            break;
        case PollResponse::BillReturned:
        {
            log(status,billType(poll.billIndex()));
        }
            break;

        }

    }
//    qDebug()<<"returned !";
    return stackedBill;
}

void CashCode::log(PollResponse::Status status, CCNet::Bill billType)
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

void CashCode::enableBillTypes(const std::vector<quint8> &params)
{
    QByteArray data((char *)params.data(),params.size());
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,data);
    qDebug()<<"enable bill types res z1: " << res.z1();
    PollResponse poll=this->pollRedundant();

}

bool CashCode::enableBillTypes(QBitArray bits, bool escrow)
{
    if(bits.size()!=24)
        return false;

    if(escrow){
        bits.resize(48);
        for(int i=0; i<24; i++){
            bits.setBit(bits[i+24]);
        }
    }
    QByteArray bytes(bits.bits(), bits.count() / 8);
    CCNetResponse res= sendCommand(CCNet::deviceCommand::enableBillTypes,0,bytes.data());
    qDebug()<<"enable bill types res z1: " << res.z1();
    PollResponse poll=this->pollRedundant();
    return true;
}

bool CashCode::requireBill(const CCNet::Bill &bill)
{
    int index=m_billTable.key(bill,-1);
    if(index==-1)
        return false;

    QBitArray bits(24,false);
    bits.setBit(index,true);

    return enableBillTypes(bits);
}

bool CashCode::requireBills(const QList<CCNet::Bill> &bills)
{
    QBitArray bits(24,false);
    for(const CCNet::Bill &bill : bills){
        int index=m_billTable.key(bill,-1);
        if(index==-1)
            return false;
        bits.setBit(index,true);
    }

    return enableBillTypes(bits);

}

bool CashCode::requireBillRedundant(const CCNet::Bill &bill)
{
    int tries=0;
    int limit=3;

    while (true) {

        try {
            return requireBill(bill);
            break;
        }
        catch (CCNetException e) {
            qDebug()<<"CashCode::requireBillRedundant exception: " << e.type() << "additional Info: " << e.additionalInfoString();

            if(e.isFatal()){
                throw e;
            }else{
                if(++tries==limit)
                    throw e;
            }
        }
    }
}

bool CashCode::requireBillsRedundant(const QList<CCNet::Bill> &bills)
{
    int tries=0;
    int limit=3;

    while (true) {

        try {
            return requireBills(bills);
            break;
        }
        catch (CCNetException e) {
            qDebug()<<"CashCode::requireBillsRedundant exception: " << e.type() << "additional Info: " << e.additionalInfoString();

            if(e.isFatal()){
                throw e;
            }else{
                if(++tries==limit)
                    throw e;
            }
        }
    }
}

void CashCode::enableBillTypesRedundant(const std::vector<quint8> &params)
{
    int tries=0;
    int limit=3;

    while (true) {

        try {
            enableBillTypes(params);
            break;
        }
        catch (CCNetException e) {
            qDebug()<<"CashCode::enableBillTypesRedundant exception: " << e.type() << "additional Info: " << e.additionalInfoString();

            if(e.isFatal()){
                throw e;
            }else{
                if(++tries==limit)
                    throw e;
            }
        }
    }
}

void CashCode::disableBillTypesRedundant()
{
    int tries=0;
    int limit=3;

    while (true) {

        try {
            disableBillTypes();
            break;
        }
        catch (CCNetException e) {
            qDebug()<<"CashCode::disableBillTypesRedundant exception: " << e.type() << "additional Info: " << e.additionalInfoString();

            if(e.isFatal()){
                throw e;
            }else{
                if(++tries==limit)
                    throw e;
            }
        }
    }
}

PollResponse CashCode::pollRedundant()
{
    int tries=0;
    int limit=3;

    while (true) {

        try {
            PollResponse res=poll();
            return res;
        }
        catch (CCNetException e) {
            qDebug()<<"CashCode::pollRedundant exception: " << e.type() << "additional Info: " << e.additionalInfoString();
            if(e.isFatal()){
                throw e;
            }else{
                this->thread()->msleep(200);
                if(++tries==limit)
                    throw e;
            }
        }
    }
}

CCNet::Bill CashCode::billType(const int index) const
{
    return m_billTable[index];
}

bool CashCode::billStacked() const
{
    return _billStacked;
}

const CCNet::BillTable &CashCode::billTable() const
{
    return m_billTable;
}

void CashCode::setBillTable(const CCNet::BillTable &newBillTable)
{
    m_billTable = newBillTable;
}

void CashCode::update(const QString &firmwarePath)
{

}
