#ifndef CASHCODE_H
#define CASHCODE_H

#include <QThread>
#include "serialport.h"
#define POLYNOMIAL 0x08408


class CashCode : public QObject
{
    Q_OBJECT
public:
    explicit CashCode(QObject *parent = nullptr);

    bool open();
    void close();

    enum offsets{
        SyncOffset =0,
        ADRoffset  =1, //03h
        LNGoffset  =2,
        CMDoffset  =3
    };

    enum class deviceCommand : quint8 {
        reset = 0x30,
        getStatus = 0x31,
        setSecurity = 0x32,
        poll = 0x33,
        enableBillTypes = 0x34,
        stackBill = 0x35,
        returnBill = 0x36,
        identification = 0x37,
        holdBill = 0x38,
        setBarcodeParameters = 0x39,
        extractBarcodeData = 0x3a,
        getBillTable = 0x41,
        download = 0x50,
        getCrc32 = 0x51,
        requeststatistics = 0x60
    };

    enum class device_state_code : quint8 {
        unknown = 0x00,
        power_up = 0x10,
        power_up_with_bill_in_val = 0x11,
        power_up_with_bill_in_stack = 0x12,
        initialize = 0x13,
        idling = 0x14,
        accepting = 0x15,
        stacking = 0x17,
        returning = 0x18,
        unit_disabled = 0x19,
        holding = 0x1a,
        device_busy = 0x1b,
        rejecting = 0x1c,
        drop_cassette_full = 0x41,
        drop_cassette_out_of_pos = 0x42,
        validator_jammed = 0x43,
        drop_cassette_jammed = 0x44,
        cheated = 0x45,
        pause = 0x46,
        failure = 0x47,
        escrow_pos = 0x80,
        bill_stacked = 0x81,
        bill_returned = 0x82
    };



    QByteArray sendCommand(QByteArray command);
    QByteArray sendCommand(const deviceCommand &cmd,const quint8 &subCmd=0, const QByteArray &data=QByteArray());

    quint16 crc16(const QByteArray data) const;

    bool messageComplete(const QByteArray &data) const;

    bool validateResponse(QByteArray data) const;

    bool sendACK();
    bool sendNAK();

    QByteArray createMessage(const deviceCommand &cmd, const quint8 &subCmd=0, QByteArray data=QByteArray());



signals:
    void error(QString error);

public slots:


private:
    SerialPort serial;
};

#endif // CASHCODE_H
