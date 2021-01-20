#ifndef CCNETRESPONSE_H
#define CCNETRESPONSE_H
#include <QByteArray>

class CCNetResponse : public QByteArray
{
public:
    CCNetResponse();

    quint8 z1() const;
    quint8 z2() const;



private:

};

#endif // CCNETRESPONSE_H
