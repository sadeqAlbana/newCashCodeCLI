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


    int dataLength() const;
    int messageLength() const;

    friend QDebug operator <<(QDebug dbg, const CCNetResponse &res);

    quint8 data(int index);
    QByteArray data() const;
    QByteArray data(int index, int length) const;

protected:
    QByteArray m_message;
    QByteArray resData;
};

QDebug operator <<(QDebug dbg, const CCNetResponse &res);



#endif // CCNETRESPONSE_H
