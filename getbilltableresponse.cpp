#include "getbilltableresponse.h"

GetBillTableResponse::GetBillTableResponse(const QByteArray &data) :CCNetResponse(data)
{

}

GetBillTableResponse::GetBillTableResponse(const CCNetResponse &other) : CCNetResponse(other)
{

}

QByteArrayList GetBillTableResponse::billTable() const
{
    QByteArrayList table;

    for(int i=0; i<119; i+=24){
        table << data(i,24);
    }
    return table;
}
