#ifndef CASHCODE_H
#define CASHCODE_H

#include <QThread>
#include "cserialport.h"
#include "utils.h"
#include "ccnetresponse.h"
#include "pollresponse.h"
class PollResponse;

class CashCode : public QObject
{
    Q_OBJECT
public:
    explicit CashCode(QString port,QObject *parent = nullptr);

    bool open();
    void close();





    CCNetResponse sendCommand(const CCNet::deviceCommand &cmd,const quint8 &subCmd=0, const QByteArray &data=QByteArray());


    bool messageComplete(const QByteArray &data) const;

    bool validateResponse(QByteArray data) const;

    bool sendACK();
    bool sendNAK();

    bool powerup();

    bool CheckErrors(QByteArray result);

    QByteArray createMessage(const CCNet::deviceCommand &cmd, const quint8 &subCmd=0, QByteArray data=QByteArray());

    void run();


    int m_LastError;


    void enableBillTypes();
    void disableBillTypes();
    PollResponse poll();

    int operate(bool &mustStop);
    void log(PollResponse::Status status, int z2);
    void log(PollResponse::Status status,PollResponse::GenericFailureReason);
    void log(PollResponse::Status status, PollResponse::RejectReason);
    void log(PollResponse::Status status);

    void enableBillTypes(int bill);

    void enableBillTypesRedundant(int bill); //will catch and rethrow exceptions
    void disableBillTypesRedundant(); //will catch and rethrow exceptions
    PollResponse pollRedundant(); //will catch and rethrow exceptions





    bool billStacked() const;

signals:
    void error(QString error);
    void stacked(int bill);
    void billEntered(bool accepting);

public slots:


private:
    CSerialPort serial;
     int         channels[8] = { 0,250,500,1000,5000,10000,25000,50000};
     bool _billStacked;
};

#endif // CASHCODE_H
