#include "identificationresponse.h"

IdentificationResponse::IdentificationResponse(const QByteArray &data) :CCNetResponse(data)
{

}

IdentificationResponse::IdentificationResponse(const CCNetResponse &other) : CCNetResponse(other.data())
{

}

QString IdentificationResponse::partNumber() const
{
    return m_data.mid(CCNet::Z1,15);
}

QString IdentificationResponse::serialNumber() const
{
    return m_data.mid(CCNet::Z1+15,12);
}

QByteArray IdentificationResponse::assetNumber() const
{
    return m_data.mid(CCNet::Z1+27,7);

}
