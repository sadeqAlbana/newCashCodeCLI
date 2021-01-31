#ifndef CCNETRESPONSE_H
#define CCNETRESPONSE_H
#include <QByteArray>
#include "utils.h"
#include <QDebug>
class CCNetResponse
{
public:
    enum Error {
        NoError      = 0,
        SyncError    = 1,
        CRCError     = 2,
        TimeoutError = 3
    };




    CCNetResponse(const QByteArray &data, const CCNetResponse::Error error);


    quint8 z1() const;
    quint8 z2() const;

    bool isValid(){return error()==NoError;}
    CCNet::deviceCommand command() const;




    Error error() const;

    int dataByteLength() const;
    int responseLength() const;

    friend QDebug operator <<(QDebug dbg, const CCNetResponse &res);


    QByteArray data() const;

protected:
    QByteArray m_data;
    Error m_error;
    int _length;



};

QDebug operator <<(QDebug dbg, const CCNetResponse &res);



#endif // CCNETRESPONSE_H
