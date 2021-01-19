#ifndef CASHCODECOMMAND_H
#define CASHCODECOMMAND_H
#include <QByteArray>
/*
format:
SYNC: 1 byte Message transmission start code [02H], fixed
ADR : 1 byte Peripheral address
LNG : 1 byte* Data length (Total number of bytes including SYNC and CRC)
CMD : 1 byte Command
DATA 0 to 250 bytes Data necessary for command (omitted if not required by CMD)
CRC: 2 bytes Check code by CRC method, LSB first
Object section to be from and including SYNC to end of DATA
(Initial value = 0)

* if a package cannot be fitted into 250-byte frame a wider frame may be used by setting LNG to 0; the actual packet
length is inserted into DATA block bytes 0 and 1 if CMD (if present in the frame) does not require subcommand,
otherwise in DATA block bytes 1 and 2; two-byte LNG always follows MSB first.



*/

/*

there are four message types

1- command transmission (Controller -> Peripheral)
2- ACK response (Peripheral <-> Controller)
3- NAK response (Peripheral -> Controller)
4- Response message (Peripheral -> Controller)
5- ILLEGAL command reponse message (Peripheral -> Controller) DATA=30H

*/

/*
Currently t free is defined as 10 mS of Bus silence, but for reliable operation of future multi-device
buses the recommended value of t free is 20mS
*/



class CashCodeCommand
{
public:


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


    CashCodeCommand(const deviceCommand &cmd,const quint8 &subCmd, const QByteArray &data);



private:
    QByteArray m_data;
};




#endif // CASHCODECOMMAND_H
