#include "ccnetresponse.h"
#include <QDebug>
#include "ccnetexception.h"
CCNetResponse::CCNetResponse(const QByteArray &data) : m_message(data),resData(m_message)
{
    resData.chop(2);
    resData.remove(0,3);
}

quint8 CCNetResponse::z1() const
{
    //return (unsigned char) m_data[3];
}

quint8 CCNetResponse::z2() const
{
    //return this->data(1);
}

quint8 CCNetResponse::data(int index)
{
    if(index>=resData.size() || index<0)
        throw CCNetException(CCNetException::OutOfRangeIndex);

    return resData[index];

}


int CCNetResponse::messageLength() const
{
    return m_message.length();
}

int CCNetResponse::dataLength() const
{
    return (int)(unsigned char) m_message[CCNet::LNGoffset];
}

QByteArray CCNetResponse::data() const
{
    return resData;
}

QByteArray CCNetResponse::data(int index, int length) const
{
    if((index+length)>data().length())
        throw CCNetException(CCNetException::OutOfRangeIndex);

    return resData.mid(index,length);
}

QDebug operator <<(QDebug dbg, const CCNetResponse &res)
{
    QDebugStateSaver saver(dbg);
    //dbg.noquote() <<" response: \n";

    dbg <<"CCNetResponse: " << res.m_message.toHex(' ');
    return dbg.noquote();
}

