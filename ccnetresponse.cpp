#include "ccnetresponse.h"
#include <QDebug>
CCNetResponse::CCNetResponse(const QByteArray &data,const CCNetResponse::Error error) : m_data(data),m_error(error)
{

}

quint8 CCNetResponse::z1() const
{
    return (unsigned char) m_data[3];
}

quint8 CCNetResponse::z2() const
{
    return (unsigned char) m_data[4];

}


CCNetResponse::Error CCNetResponse::error() const
{
    return m_error;
}

int CCNetResponse::responseLength() const
{
    return data().length();
}

int CCNetResponse::dataByteLength() const
{
    return (int)(unsigned char) data().at(CCNet::LNGoffset);
}

QByteArray CCNetResponse::data() const
{
    return m_data;
}

QDebug operator <<(QDebug dbg, const CCNetResponse &res)
{
    QDebugStateSaver saver(dbg);
    //dbg.noquote() <<" response: \n";

    dbg.noquote() << res.m_data.toHex(' ');
    return dbg.noquote();
}

