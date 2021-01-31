#ifndef GETBILLTABLERESPONSE_H
#define GETBILLTABLERESPONSE_H
#include "ccnetresponse.h"

class GetBillTableResponse : public CCNetResponse
{
public:
    GetBillTableResponse(const QByteArray &data, const CCNetResponse::Error error);
    GetBillTableResponse(const CCNetResponse &other);

    QByteArrayList billTable() const;
};

#endif // GETBILLTABLERESPONSE_H
