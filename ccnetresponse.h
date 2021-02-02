#ifndef CCNETRESPONSE_H
#define CCNETRESPONSE_H
#include <QByteArray>
#include "utils.h"
#include <QDebug>
class CCNetResponse
{
public:



    CCNetResponse(const QByteArray &data);


    quint8 z1() const;
    quint8 z2() const;

    CCNet::deviceCommand command() const;





    int dataByteLength() const;
    int responseLength() const;

    friend QDebug operator <<(QDebug dbg, const CCNetResponse &res);


    QByteArray data() const;

protected:
    QByteArray m_data;
    int _length;
};

QDebug operator <<(QDebug dbg, const CCNetResponse &res);



#endif // CCNETRESPONSE_H
