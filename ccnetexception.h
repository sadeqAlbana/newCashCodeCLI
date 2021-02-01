#ifndef CCNETEXCEPTION_H
#define CCNETEXCEPTION_H
#include <QDebug>

class CCNetException
{
public:
    enum Type{
        SerialWriteTimeout,
        SerialReadTimeout,
        IllegalCommand,
        IncompleteResponse,
        NAK,
        CRCError,
        SyncError,

    };


    CCNetException();

    friend QDebug operator<<(QDebug debug, CCNetException::Type type );




};

QDebug operator<<(QDebug debug, CCNetException::Type type );


#endif // CCNETEXCEPTION_H
