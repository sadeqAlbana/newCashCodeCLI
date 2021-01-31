#include "getbilltableresponse.h"

GetBillTableResponse::GetBillTableResponse(const QByteArray &data, const CCNetResponse::Error error) :CCNetResponse(data,error)
{

}

GetBillTableResponse::GetBillTableResponse(const CCNetResponse &other) : CCNetResponse(other.data(),other.error())
{

}

QByteArrayList GetBillTableResponse::billTable() const
{
    QByteArrayList table;

    for(int i=CCNet::Z1; i<CCNet::Z1+119; i+=24){
        table << m_data.mid(i,24);
    }
    return table;
}
