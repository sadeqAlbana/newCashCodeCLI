#include "identificationresponse.h"

IdentificationResponse::IdentificationResponse(const QByteArray &data) :CCNetResponse(data)
{

}

IdentificationResponse::IdentificationResponse(const CCNetResponse &other) : CCNetResponse(other)
{

}

QString IdentificationResponse::partNumber() const
{
    return data(0,15);
}

QString IdentificationResponse::serialNumber() const
{
    return data(15,12);
}

QByteArray IdentificationResponse::assetNumber() const
{
    return data(27,7);
}
