#ifndef GETSTATUSRESPONSE_H
#define GETSTATUSRESPONSE_H
#include "ccnetresponse.h"

class GetStatusResponse : public CCNetResponse
{
public:
    GetStatusResponse(const QByteArray &data);
    GetStatusResponse(const CCNetResponse &other);


    QByteArray billTypes() const; //z1-z3
    QByteArray securityLevels() const; //z4-z6



};

#endif // GETSTATUSRESPONSE_H
