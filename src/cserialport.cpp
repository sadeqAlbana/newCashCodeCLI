#include "cserialport.h"
#include <QCoreApplication>
CSerialPort::CSerialPort(QObject *parent) : QSerialPort(parent)
{
    setBaudRate(9600);
    setDataBits(DataBits::Data8);    //it's set by default, but we set it anyway in case default changes
    setStopBits(StopBits::OneStop);  //it's set by default, but we set it anyway in case default changes
    setParity(Parity::NoParity);    //it's set by default, but we set it anyway in case default changes
    setFlowControl(FlowControl::NoFlowControl); //check this later

    connect(QCoreApplication::instance(),&QCoreApplication::aboutToQuit,this,&CSerialPort::close);
}
