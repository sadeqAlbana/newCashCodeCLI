#include "getbilltableresponse.h"

GetBillTableResponse::GetBillTableResponse(const QByteArray &data) :CCNetResponse(data)
{

}

GetBillTableResponse::GetBillTableResponse(const CCNetResponse &other) : CCNetResponse(other.data())
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
