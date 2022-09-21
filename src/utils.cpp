#include "utils.h"
#include <QDebug>

quint16 Utils::crc16(const QByteArray &data)
{
    unsigned int CRC;
    unsigned char j;
    CRC = 0;
    for(int i=0; i < data.size(); i++)
    {
        CRC ^= (unsigned char)data[i];
        for(j=0; j < 8; j++)
        {
            if(CRC & 0x0001) {CRC >>= 1; CRC ^= POLYNOMIAL;}
            else CRC >>= 1;
        }
    }
    return CRC;
}

CCNet::Bill::Bill(const QString &currency, const int value) : m_currency(currency),m_value(value)
{

}

CCNet::Bill::Bill(): m_currency(QString()),m_value(0)
{

}

bool CCNet::Bill::operator ==(const Bill &other) const
{
    return this->m_currency==other.m_currency &&
            this->m_value==other.m_value;
}



const QString &CCNet::Bill::currency() const
{
    return m_currency;
}

int CCNet::Bill::value() const
{
    return m_value;
}

bool CCNet::Bill::isValid() const
{
    return !m_currency.isEmpty() && m_value>0;
}

QDebug operator<<(QDebug debug, const CCNet::Bill &bill)

{
    debug << QString("%1%2").arg(bill.value()).arg(bill.currency());

    return debug;
}


