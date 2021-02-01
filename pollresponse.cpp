#include "pollresponse.h"

PollResponse::PollResponse(const QByteArray &data, const CCNetResponse::Error error) : CCNetResponse (data,error)
{

}

PollResponse::PollResponse(const CCNetResponse &other) : CCNetResponse (other)
{

}

PollResponse::Status PollResponse::status() const
{
    qDebug()<<PollResponse::Status(z1());
    return PollResponse::Status(z1());
}

PollResponse::GenericFailureReason PollResponse::failureReason() const
{
    return PollResponse::GenericFailureReason(z2());

}

int PollResponse::stackedBill() const
{
    return CCNet::channels[z2()];
}


QDebug operator<<(QDebug debug, PollResponse::Status status)
{
    switch (status) {
    case PollResponse::PowerUp:
        debug << "PollResponse::PowerUp";
        break;
    case PollResponse::PowerUpWithBillinValidator:
        debug << "PollResponse::PowerUpWithBillinValidator";
        break;
    case PollResponse::PowerUpWithBillinStacker:
        debug << "PollResponse::PowerUpWithBillinStacker";
        break;
    case PollResponse::Initilize:
        debug << "PollResponse::Initilize";
        break;
    case PollResponse::Idling:
        debug << "PollResponse::Idling";
        break;
    case PollResponse::Accepting:
        debug << "PollResponse::Accepting";
        break;
    case PollResponse::Stacking:
        debug << "PollResponse::Stacking";
        break;
    case PollResponse::Returning:
        debug << "PollResponse::Returning";
        break;
    case PollResponse::Disabled:
        debug << "PollResponse::Disabled";
        break;
    case PollResponse::Holding:
        debug << "PollResponse::Holding";
        break;
    case PollResponse::Busy:
        debug << "PollResponse::Busy";
        break;
    case PollResponse::Rejecting:
        debug << "PollResponse::Rejecting";
        break;
    case PollResponse::DropCassetteFull:
        debug << "PollResponse::DropCassetteFull";
        break;
    case PollResponse::DropCassetteOutOfPosition:
        debug << "PollResponse::DropCassetteOutOfPosition";
        break;
    case PollResponse::ValidatorJammed:
        debug << "PollResponse::ValidatorJammed";
        break;
    case PollResponse::DropCassetteJammed:
        debug << "PollResponse::DropCassetteJammed";
        break;
    case PollResponse::Cheated:
        debug << "PollResponse::Cheated";
        break;
    case PollResponse::Pause:
        debug << "PollResponse::Pause";
        break;
    case PollResponse::GenericFailure:
        debug << "PollResponse::GenericFailure";
        break;
    case PollResponse::EscrowPosition:
        debug << "PollResponse::EscrowPosition";
        break;
    case PollResponse::BillStacked:
        debug << "PollResponse::BillStacked";
        break;
    case PollResponse::BillReturned:
        debug << "PollResponse::BillReturned";
        break;

    default:
        debug << QString("PollResponse::Status: Unknown value: %1 ").arg(QString(QByteArray::fromRawData((char *)&status,1).toHex()));
        break;
    }
    return debug;

}

QDebug operator<<(QDebug debug, PollResponse::GenericFailureReason failure)
{

//        StackMotorFailure = 0x50,
//        TransportMotorSpeedFailure = 0x51,
//        TransportMotorFailure = 0x52,
//        AligningMotorFailure = 0x53,
//        InitialCassetteStatusFailure = 0x54,
//        OpticalCanalFailure = 0x55,
//        MagenaticCanalFailure = 0x56,
//        CapacitanceCanalFailure = 0x5F,
//    };

    switch (failure) {
    case PollResponse::StackMotorFailure:
        debug << "PollResponse::StackMotorFailure";
        break;
    case PollResponse::TransportMotorSpeedFailure:
        debug << "PollResponse::TransportMotorSpeedFailure";
        break;
    case PollResponse::TransportMotorFailure:
        debug << "PollResponse::TransportMotorFailure";
        break;
    case PollResponse::AligningMotorFailure:
        debug << "PollResponse::AligningMotorFailure";
        break;
    case PollResponse::InitialCassetteStatusFailure:
        debug << "PollResponse::InitialCassetteStatusFailure";
        break;
    case PollResponse::OpticalCanalFailure:
        debug << "PollResponse::OpticalCanalFailure";
        break;
    case PollResponse::MagenaticCanalFailure:
        debug << "PollResponse::MagenaticCanalFailure";
        break;
    case PollResponse::CapacitanceCanalFailure:
        debug << "PollResponse::CapacitanceCanalFailure";
        break;

    default:
        debug << QString("PollResponse::GenericFailureReason Unknown value: %1 ").arg(QString(QByteArray::fromRawData((char *)&failure,1).toHex()));
    }
    return debug;
}
