#ifndef CASHCODE_H
#define CASHCODE_H

#include <QThread>
#include "serialport.h"
class CashCode : public QObject
{
    Q_OBJECT
public:
    explicit CashCode(QObject *parent = nullptr);

    bool open();
    bool close();

    QByteArray sendCommand(QByteArray command);

signals:
    void error(QString error);

public slots:


private:
    SerialPort serial;
};

#endif // CASHCODE_H
