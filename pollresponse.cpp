#include "pollresponse.h"

PollResponse::PollResponse(const QByteArray &data) : CCNetResponse (data)
{

}

PollResponse::PollResponse(const CCNetResponse &other) : CCNetResponse (other)
{

}

PollResponse::Status PollResponse::status() const
{
    return PollResponse::Status(z1());
}

PollResponse::GenericFailureReason PollResponse::failureReason() const
{
    return PollResponse::GenericFailureReason(z2());
}

PollResponse::RejectReason PollResponse::rejectReason() const
{
    return PollResponse::RejectReason(z2());
}

int PollResponse::stackedBill() const
{
    return CCNet::channels[z2()];
}

QDebug operator<<(QDebug debug, PollResponse::RejectReason reason)
{

    switch (reason) {
    case PollResponse::InsertionError: debug << "PollResponse::InsertionError"; break;
    case PollResponse::MagenaticError: debug << "PollResponse::MagenaticError"; break;
    case PollResponse::BillInTheHead:  debug << "PollResponse::BillInTheHead"; break;
    case PollResponse::Multiplying: debug << "PollResponse::Multiplying"; break;
    case PollResponse::ConveyingError: debug << "PollResponse::ConveyingError"; break;
    case PollResponse::IdentificationError: debug << "PollResponse::IdentificationError"; break;
    case PollResponse::VerificationError:debug << "PollResponse::VerificationError"; break;
    case PollResponse::OpticError: debug << "PollResponse::OpticError"; break;
    case PollResponse::InhibitDenominationError:  debug << "PollResponse::InhibitDenominationError"; break;
    case PollResponse::CapacitanceError:debug << "PollResponse::CapacitanceError"; break;
    case PollResponse::OperationError:debug << "PollResponse::OperationError"; break;
    case PollResponse::LengthError: debug << "PollResponse::LengthError"; break;
    case PollResponse::UnrecognisedBarcode: debug << "PollResponse::UnrecognisedBarcode"; break;
    case PollResponse::UvOptic: debug << "PollResponse::UvOptic"; break;
    case PollResponse::IncorrectNumberOfCharactersInBarcode: debug << "PollResponse::IncorrectNumberOfCharactersInBarcode"; break;
    case PollResponse::UnknownBarcodeStartSequence:  debug << "PollResponse::UnknownBarcodeStartSequence"; break;
    case PollResponse::UnknownBarcodeStopSequence: debug << "PollResponse::UnknownBarcodeStopSequence"; break;

    default:
        debug << QString("PollResponse::RejectReason: Unknown value: %1 ").arg(QString(QByteArray::fromRawData((char *)&reason,1).toHex()));
        break;
    }
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
