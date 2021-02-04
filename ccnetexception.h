#ifndef CCNETEXCEPTION_H
#define CCNETEXCEPTION_H
#include <QDebug>
#include <QJsonObject>
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
        OutOfRangeIndex,
        GenericFailure

    };


    CCNetException(const CCNetException::Type &type,const QString &method=QString(),const QJsonObject &additionalInfo=QJsonObject());

    friend QDebug operator<<(QDebug debug, CCNetException::Type type );


    CCNetException::Type type() const;


    QString callerMethod() const;

    QJsonObject additionalInfo() const;

private:
    Type m_type;
    QString m_description;
    QString m_callerMethod;
    QJsonObject m_additionalInfo;





};

QString toString(CCNetException::Type type);


QDebug operator<<(QDebug debug, CCNetException::Type type );


#endif // CCNETEXCEPTION_H
