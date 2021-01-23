#include "cashcode.h"
#include <QDeadlineTimer>
#include <QDebug>
#include <QCoreApplication>
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



QByteArray CashCode::sendCommand(const CashCode::deviceCommand &cmd, const quint8 &subCmd, const QByteArray &data)
{

    QByteArray message=createMessage(cmd,subCmd,data);

    serial.clear();


    if(!serial.write(message))
        return QByteArray();

    if(!serial.waitForBytesWritten()) //official is 10ms
        return QByteArray("WBR");

    unsigned long utimeout= message.at(3)=='\x30' ? 2000000 : 10000;


    QThread::usleep(utimeout); //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

    // start reading response


    int timeout= message.at(3)=='\x30' ? 3000 : 3000;

    QByteArray result;
    if(serial.waitForReadyRead(timeout)){ //tresponse(max.) 10.0 msec : The maximum time Peripheral will take to respond to a valid communication

        result.append(serial.readAll());
        if(result[SyncOffset]!='\x02'){
            sendNAK();
            //qDebug()<<result;
            return QByteArray("WR1");
        }


        while(!messageComplete(result)){ //tinter-byte(max.) 5.0 msec : The maximum time allowed between bytes in a block transmission
            if(!serial.waitForReadyRead(10)){
                sendNAK();
                //qDebug()<<result;
                return QByteArray("TO");
            }
            result.append(serial.readAll());
        }

        //result is now complete, check crc16
        bool crcValid=validateResponse(result);
        if(!crcValid){
            sendNAK();
            //qDebug()<<result;
            return QByteArray();
        }
        //qDebug()<<"crc validation: " << crcValid;


        //QThread::usleep(20000); //20ms
        sendACK();
        //qDebug()<<result;
        return result;
    }
    else{

        //QThread::usleep(20000); //20ms
        sendNAK();
        //qDebug()<<result;
        return QByteArray("WR2");
    }


}



unsigned int CashCode::crc16(const QByteArray data) const
{
    unsigned int CRC;
    unsigned char j;
    CRC = 0;
    for(int i=0; i < data.size(); i++)
    {
        CRC ^= (unsigned char)data[i];
        for(j=0; j < 8; j++)
        {
            if(CRC & 0x0001) {CRC >>= 1; CRC ^= POLYNOMIAL;}
            else CRC >>= 1;
        }
    }
    return CRC;
}

bool CashCode::messageComplete(const QByteArray &data) const
{
    if(data.size()>=3){
        int length=data[LNGoffset];
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
//    crc.append(data[data.size()-1]);
//    crc.append(data[data.size()-2]);

    quint16 messageCRC=crc.toHex().toUInt(&ok,16);
    if(!ok)
        return false;

    data.chop(2);
    return (messageCRC==crc16(data));

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

    // Power UP
    QByteArray result;
    result = sendCommand(deviceCommand::poll);

    if (result.size()>0)
        qDebug()<<"POWER UP: "<< result;

    // Check result for errors
//    if(this->CheckErrors(result)){
//        //this->sendCommand(deviceCommand::NAK);
//    }

    // Если CashCode вернул в 4 байте 0х19 значит он уже включен
    if(result[3] == '\x19') {
        qDebug() << "Validator ready to work!";
        return 0;
    }

    // Если все хорошо, отправляет команду подтверждения
    //this->sendCommand(deviceCommand::ACK);

    // RESET
    result = this->sendCommand(deviceCommand::reset);
     qDebug()<<"RESET: " << result;

    // Если купюроприемник не ответил сигналом ACK
    if(result[3] != '\x00'){
        m_LastError = 0x00;
        return m_LastError;
    }


    // Опрос купюроприемника процедура инициализации
    result = this->sendCommand(deviceCommand::poll);
    qDebug()<<"POLL: "<< result;
//    if(this->CheckErrors(result)){
//        this->sendCommand(deviceCommand::NAK);
//    }
//    this->sendCommand(deviceCommand::ACK);


    // Получение статуса GET_STATUS
    result = this->sendCommand(deviceCommand::getStatus);
    qDebug()<<"GET_STATUS: " <<result;

    // Команда GET STATUS возвращает 6 байт ответа. Если все равны 0, то статус ok и можно работать дальше, иначе ошибка
    if(result[3] != '\x00' || result[4] != '\x00' || result[5] != '\x00' ||
       result[6] != '\x00' || result[7] != '\x00' || result[8] != '\x00' )
    {
        this->m_LastError = 0x70;
    }

    // Подтверждает если все хорошо
    //this->sendCommand(deviceCommand::ACK);

    // SET_SECURITY (в тестовом примере отправояет 3 байта (0 0 0)
    result = this->sendCommand(deviceCommand::setSecurity,0, QByteArray::fromHex("000000"));
   qDebug()<<"set security: " <<result;

    // Если не получили от купюроприемника сигнал ACK
    if(result[3] != '\x00'){
        m_LastError = 0x00;
        return this->m_LastError;
    }

    // IDENTIFICATION
    result = this->sendCommand(deviceCommand::identification);
    //this->sendCommand(deviceCommand::ACK);
    //print_b("IDENTIFICATION: ", result);

    // Опрашиваем купюроприемник должны получить команду INITIALIZE
    result = this->sendCommand(deviceCommand::poll);
    //print_b("INITIALIZE: ", result);

//    if(this->CheckErrors(result)){
//        this->sendCommand(deviceCommand::NAK);
//    }

   // this->sendCommand(deviceCommand::ACK);

    // POLL Должны получить команду UNIT DISABLE
    result = this->sendCommand(deviceCommand::poll);
    //print_b("UNIT DISABLE: ", result);

//    if(this->CheckErrors(result)){
//        this->sendCommand(deviceCommand::NAK);
//        return m_LastError;
//    }

//    this->sendCommand(deviceCommand::ACK);
    //this->m_IsPowerUp = true;

    return this->m_LastError;
}

bool CashCode::CheckErrors(QByteArray result)
{
    bool is_error = false;

    switch(result[3]) {
    case 0x30:
        this->m_LastError = 0x30;
        is_error = true;
        break;
    case 0x40:
        m_LastError = 0x40;
        is_error = true;
        break;
    case 0x42:
        m_LastError = 0x42;
        is_error = true;
        break;
    case 0x43:
        m_LastError = 0x43;
        is_error = true;
        break;
    case 0x44:
        m_LastError = 0x44;
        is_error = true;
        break;
    case 0x45:
        m_LastError = 0x45;
        is_error = true;
        break;
    case 0x46:
        m_LastError = 0x46;
        is_error = false;
        break;
    case 0x47:
        switch(result[4]) {
            case 0x50: m_LastError = 0x50; break;
            case 0x51: m_LastError = 0x51; break;
            case 0x52: m_LastError = 0x52; break;
            case 0x53: m_LastError = 0x53; break;
            case 0x54: m_LastError = 0x54; break;
            case 0x55: m_LastError = 0x55; break;
            case 0x56: m_LastError = 0x56; break;
            case 0x5F: m_LastError = 0x5F; break;
        default:
            m_LastError = 0x47;
        }
        is_error = true;
        break;
    }

    if(is_error)
        qDebug()<< "ERROR: " << QByteArray::fromRawData((char *)&m_LastError,1).toHex();
    return is_error;
}

QByteArray CashCode::createMessage(const CashCode::deviceCommand &cmd, const quint8 &subCmd, QByteArray data)
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

    quint16 crc=crc16(message);
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
            QByteArray result = sendCommand(deviceCommand::poll);
            qDebug()<<"res: "<<result;
            //qDebug()<<Q_FUNC_INFO << result << " ---------" <<"SIZE: " << result.size() << " -- " << (result[3]==0xff);
            if (result.size() < 4){
                //this->_ccnet->sendCommand(deviceCommand::NAK);
                has_error=true;
                qDebug()<<"Has error";
                continue;
            }
            if (this->CheckErrors(result)){
                has_error = true;
                must_stop = true;
                qDebug()<<"balance before Error: " << balance;
                balance=0;
//                CashCodeError cc_error(this->_ccnet->m_LastError);
//                QString error_string(cc_error.getMessage().c_str());
//                KLog::log("error/cashcode",error_string);
//                this->_ccnet->sendCommand(deviceCommand::NAK);
                continue;
            }
//            if (result[3] == '\x00'){
//                //this->_ccnet->sendCommand(deviceCommand::POLL);
//                continue;
//            }
//            if (result[3] == '\x14'){
//                //this->_ccnet->sendCommand(deviceCommand::ACK);
//                continue;
//            }

            if (result[3] != '\x14'){
//                if (result[3] == 0x15){
//                    // Accepting
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
//                else if (result[3] == 0x1c){
//                    // ESCROW Position
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
                 if (result[3] == '\x80'){
                    //this->_ccnet->sendCommand(deviceCommand::ACK);
                    qDebug()<<"Stack sent !";
                    qDebug()<<"rr:" << sendCommand(deviceCommand::stackBill);
                }
//                else if (result[3] == 0x17){
//                    this->_ccnet->sendCommand(deviceCommand::ACK);
//                }
                else if (result[3] == '\x81'){
                    //Bill Stacked
                    qDebug()<<"Bill Stacked:";
                    stackCounter++;
                    qDebug()<<"stack counter: " << stackCounter;
                    //if(stackCounter==3){
                        //this->_ccnet->sendCommand(deviceCommand::ACK);
                        balance = this->channels[result[4]];
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
