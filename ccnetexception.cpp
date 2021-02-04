#include "ccnetexception.h"



CCNetException::CCNetException(const CCNetException::Type &type, const QString &method, const QJsonObject &additionalInfo) : m_type(type),
    m_callerMethod(method),
    m_additionalInfo(additionalInfo)
{

}

CCNetException::Type CCNetException::type() const
{
    return m_type;
}



QString CCNetException::callerMethod() const
{
    return m_callerMethod;
}

QJsonObject CCNetException::additionalInfo() const
{
    return m_additionalInfo;
}
QString toString(CCNetException::Type type)

{
    switch (type) {
    case CCNetException::SerialWriteTimeout:
        return "CCNetException::SerialWriteTimeout";

    case CCNetException::SerialReadTimeout:
        return "CCNetException::SerialReadTimeout";

    case CCNetException::SerialClearError:
        return "CCNetException::SerialClearError";

    case CCNetException::IllegalCommand:
        return "CCNetException::IllegalCommand";

    case CCNetException::IncompleteResponseTimout:
        return "CCNetException::IncompleteResponseTimout";

    case CCNetException::NAK:
        return "CCNetException::NAK";

    case CCNetException::CRCError:
        return "CCNetException::CRCError";

    case CCNetException::SyncError:
        return "CCNetException::SyncError";

    case CCNetException::OutOfRangeIndex:
        return "CCNetException::OutOfRangeIndex";

    case CCNetException::GenericFailure:
        return "CCNetException::GenericFailure";

    }
}


QDebug operator<<(QDebug debug, CCNetException::Type type)

{
    debug << toString(type);

    return debug;
}
