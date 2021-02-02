#ifndef CCNETEXCEPTION_H
#define CCNETEXCEPTION_H
#include <QDebug>

class CCNetException
{
public:
    enum Type{
        SerialWriteTimeout,
        SerialReadTimeout,
        SerialClearError,
        IllegalCommand,
        IncompleteResponseTimout, //received response but timedout later
        NAK,
        CRCError,
        SyncError,
        OutOfRangeIndex

    };


    CCNetException(const CCNetException::Type &type);

    friend QDebug operator<<(QDebug debug, CCNetException::Type type );


    CCNetException::Type type() const;

private:
    Type m_type;




};

QDebug operator<<(QDebug debug, CCNetException::Type type );


#endif // CCNETEXCEPTION_H
