#include "ccnetresponse.h"
#include <QDebug>
#include "ccnetexception.h"
CCNetResponse::CCNetResponse(const QByteArray &data) : m_message(data),resData(data)
{
    m_message.detach();
    resData.detach();
    resData.chop(2);
    resData.remove(0,3);
}

quint8 CCNetResponse::z1() const
{
    return data(0);
}

quint8 CCNetResponse::z2() const
{
    return data(1);
}

quint8 CCNetResponse::data(int index) const
{
    if(index>=resData.size() || index<0)
        throw CCNetException(CCNetException::OutOfRangeIndex,Q_FUNC_INFO,QJsonObject{{"index",index},{"data_size",dataLength()},
                             {"message_size",messageLength()}});

    return (quint8)resData[index];

}


int CCNetResponse::messageLength() const
{
    return m_message.length();
}

int CCNetResponse::dataLength() const
{
    return resData.length();
}

QByteArray CCNetResponse::data() const
{
    return resData;
}

QByteArray CCNetResponse::data(int index, int length) const
{
    if((index+length)>data().length())
        throw CCNetException(CCNetException::OutOfRangeIndex,Q_FUNC_INFO,QJsonObject{{"index",index},{"length",length},
                                                                                     {"data_size",dataLength()},
                             {"message_size",messageLength()}});

    return resData.mid(index,length);
}

QByteArray CCNetResponse::message() const
{
    return m_message;
}

QDebug operator <<(QDebug dbg, const CCNetResponse &res)
{
    QDebugStateSaver saver(dbg);
    //dbg.noquote() <<" response: \n";

    dbg <<"CCNetResponse: " << res.m_message.toHex(' ');
    return dbg.noquote();
}

