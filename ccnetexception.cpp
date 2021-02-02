#include "ccnetexception.h"

CCNetException::CCNetException(const CCNetException::Type &type) : m_type(type)
{

}

CCNetException::Type CCNetException::type() const
{
    return m_type;
}

QDebug operator<<(QDebug debug, CCNetException::Type type)
{
    switch (type) {
    case CCNetException::SerialWriteTimeout:
        debug << "CCNetException::SerialWriteTimeout";
        break;
    case CCNetException::SerialReadTimeout:
        debug << "CCNetException::SerialReadTimeout";
        break;
    case CCNetException::SerialClearError:
        debug << "CCNetException::SerialClearError";
        break;
    case CCNetException::IllegalCommand:
        debug << "CCNetException::IllegalCommand";
        break;
    case CCNetException::IncompleteResponseTimout:
        debug << "CCNetException::IncompleteResponseTimout";
        break;
    case CCNetException::NAK:
        debug << "CCNetException::NAK";
        break;
    case CCNetException::CRCError:
        debug << "CCNetException::CRCError";
        break;
    case CCNetException::SyncError:
        debug << "CCNetException::SyncError";
        break;
    }
    return debug;
}


