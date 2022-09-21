#include "getstatusresponse.h"

GetStatusResponse::GetStatusResponse(const QByteArray &data) : CCNetResponse (data)
{

}

GetStatusResponse::GetStatusResponse(const CCNetResponse &other) : CCNetResponse (other)
{

}

QByteArray GetStatusResponse::billTypes() const
{
    return data(0,3);
}

QByteArray GetStatusResponse::securityLevels() const
{
    return data(3,3);

}
