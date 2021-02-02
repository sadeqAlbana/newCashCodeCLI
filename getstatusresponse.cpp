#include "getstatusresponse.h"

GetStatusResponse::GetStatusResponse(const QByteArray &data) : CCNetResponse (data)
{

}

GetStatusResponse::GetStatusResponse(const CCNetResponse &other) : CCNetResponse (other)
{

}

QByteArray GetStatusResponse::billTypes() const
{
    return m_data.mid(CCNet::Z1,3);
}

QByteArray GetStatusResponse::securityLevels() const
{
    return m_data.mid(CCNet::Z1+3,3);

}
