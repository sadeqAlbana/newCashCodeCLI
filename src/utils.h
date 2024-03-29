#ifndef UTILS_H
#define UTILS_H

#include <QByteArray>
#include <QString>
#include <QList>
#define POLYNOMIAL 0x08408
namespace CCNet {

class Bill{
public:
    Bill(const QString &currency, const int value);
    Bill();

    bool operator ==(const Bill &other) const;

    const QString &currency() const;

    int value() const;
    bool isValid() const;

    operator QString(){
        return QString("%1%2").arg(m_currency,m_value);
    }

private:

    QString m_currency;
    int m_value;
};

using BillTable = QMap<int, CCNet::Bill>;

enum Bytes : quint8 {
    SyncByte = 0x02,
    AddressByte = 0x03,
    ACKByte     = 0x00,
    NAKByte     = 0xFF
};

enum offsets{
    SyncOffset =0,
    ADRoffset  =1, //03h
    LNGoffset  =2,
//    CMDoffset  =3,
    Z1         =3
};

enum class deviceCommand : quint8 {
    reset = 0x30, //ALL
    getStatus = 0x31, //ALL
    setSecurity = 0x32, //ALL
    poll = 0x33, //ALL
    enableBillTypes = 0x34, //ALL
    stackBill = 0x35, //ESCROW, HOLDING,
    returnBill = 0x36, //ESCROW, HOLDING,
    identification = 0x37, //INITIALIZE, DISABLED, FAILURE
    holdBill = 0x38, //ESCROW, HOLDING, must call every 10 seconds or bill will be returned
    setBarcodeParameters = 0x39,
    extractBarcodeData = 0x3a,
    getBillTable = 0x41, //INITIALIZE, DISABLED, FAILURE
    download = 0x50, //INITIALIZE, DISABLED, FAILURE
    getFirmwareCrc32 = 0x51, ////INITIALIZE, DISABLED, FAILURE
    requeststatistics = 0x60 ////INITIALIZE, DISABLED, FAILURE
};

/*
If a Controller command is received by the Bill Validator, which is not executable in its present state, the Bill Validator
issues ILLEGAL COMMAND message.
*/

/*
The IDENTIFICATION, GET BILL TABLE, DOWNLOAD and REQUEST STATISTICS commands should be sent by the
Controller when Bill Validator is in the following states: Power up, Initialize, one of the Failure states (41H-47H) or Unit
Disabled. Otherwise an ILLEGAL COMMAND response will be returned.
*/

}

//bool operator != (const char first, const CCNet::Bytes second){
//    return ((quint8) first != (quint8)second);
//}

namespace Utils {
    quint16 crc16(const QByteArray &data);
}

QDebug operator<<(QDebug debug, const CCNet::Bill &bill );


#endif // UTILS_H
