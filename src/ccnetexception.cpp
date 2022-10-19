#include "ccnetexception.h"
#include <QJsonDocument>


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

QString CCNetException::additionalInfoString() const
{
    return QJsonDocument(additionalInfo()).toJson();
}

bool CCNetException::isFatal()
{
    switch (type()) {
    case CCNetException::CRCError: return false;
    case CCNetException::SyncError: return false;
    case CCNetException::IncompleteResponseTimout: return false;
    case CCNetException::NAK: return false;

    default: return true;

    }
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
        return QStringLiteral("CCNetException::SyncError");

    case CCNetException::OutOfRangeIndex:
        return QStringLiteral("CCNetException::OutOfRangeIndex");

    case CCNetException::GenericFailure:
        return QStringLiteral("CCNetException::GenericFailure");
    case CCNetException::RejectionError:
        return QStringLiteral("CCNetException::RejectionError");
    case CCNetException::ValidatorJammedError:
        return QStringLiteral("CCNetException::ValidatorJammedError");
    case CCNetException::DropCassetteJammedError:
        return QStringLiteral("CCNetException::DropCassetteJammedError");
    case CCNetException::DropCassetteFullError:
        return QStringLiteral("CCNetException::DropCassetteFullError");
    case CCNetException::PausedError:
        return QStringLiteral("CCNetException::PausedError");
    case CCNetException::DropCassetteOutOfPositionError:
        return QStringLiteral("CCNetException::DropCassetteOutOfPositionError");

//    case CCNetException::CheatedError:
//        return "CCNetException::CheatedError";
    }

    return QStringLiteral("CCNetException::Invalid enum supplied");
}


QDebug operator<<(QDebug debug, CCNetException::Type type)

{
    debug << toString(type);

    return debug;
}
