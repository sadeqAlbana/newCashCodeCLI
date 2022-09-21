#ifndef CSERIALPORT_H
#define CSERIALPORT_H

#include <QSerialPort>

class CSerialPort : public QSerialPort
{
    Q_OBJECT
public:
    explicit CSerialPort(QObject *parent = nullptr);

signals:

public slots:


};

#endif // CSERIALPORT_H
