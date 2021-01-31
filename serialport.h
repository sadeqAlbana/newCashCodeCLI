#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>

class SerialPort : public QSerialPort
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);

signals:

public slots:


};

#endif // SERIALPORT_H
