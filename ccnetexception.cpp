#include "ccnetexception.h"

CCNetException::CCNetException()
{

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
    case CCNetException::IllegalCommand:
        debug << "CCNetException::IllegalCommand";
        break;
    case CCNetException::IncompleteResponse:
        debug << "CCNetException::IncompleteResponse";
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
