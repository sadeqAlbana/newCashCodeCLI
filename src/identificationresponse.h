#ifndef IDENTIFICATIONRESPONSE_H
#define IDENTIFICATIONRESPONSE_H
#include "ccnetresponse.h"


/*

Z1-Z15 Part Number – 15 bytes, ASCII characters
Z16-Z27 Serial Number – 12 bytes Factory assigned serial number, ASCII characters
Z28-Z34 Asset Number – 7 bytes, unique to every Bill Validator, binary data

Bytes Z1-Z27 must be sent as ASCII Characters. Zero (30H) and Blank (20H) are acceptable. Asset Number must be sent
as binary code.

*/

class IdentificationResponse : public CCNetResponse
{
public:
    IdentificationResponse(const QByteArray &data);
    IdentificationResponse(const CCNetResponse &other);

    QString partNumber() const;
    QString serialNumber() const;
    QByteArray assetNumber() const;
};

#endif // IDENTIFICATIONRESPONSE_H
