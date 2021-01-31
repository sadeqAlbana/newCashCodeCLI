#include "utils.h"


quint16 Utils::crc16(const QByteArray &data)
{
    unsigned int CRC;
    unsigned char j;
    CRC = 0;
    for(int i=0; i < data.size(); i++)
    {
        CRC ^= (unsigned char)data[i];
        for(j=0; j < 8; j++)
        {
            if(CRC & 0x0001) {CRC >>= 1; CRC ^= POLYNOMIAL;}
            else CRC >>= 1;
        }
    }
    return CRC;
}
