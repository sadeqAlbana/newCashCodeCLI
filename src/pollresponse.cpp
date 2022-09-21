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

int PollResponse::billIndex() const
{
    return z2();
}

QDebug operator<<(QDebug debug, PollResponse::RejectReason reason)
{
    debug << toString(reason);
    return debug;
}


QString toString(PollResponse::RejectReason reason)

{

    switch (reason) {
    case PollResponse::InsertionError: return "PollResponse::InsertionError"; break;
    case PollResponse::MagenaticError: return "PollResponse::MagenaticError"; break;
    case PollResponse::BillInTheHead:  return "PollResponse::BillInTheHead"; break;
    case PollResponse::Multiplying: return "PollResponse::Multiplying"; break;
    case PollResponse::ConveyingError: return "PollResponse::ConveyingError"; break;
    case PollResponse::IdentificationError: return "PollResponse::IdentificationError"; break;
    case PollResponse::VerificationError:return "PollResponse::VerificationError"; break;
    case PollResponse::OpticError: return "PollResponse::OpticError"; break;
    case PollResponse::InhibitDenominationError:  return "PollResponse::InhibitDenominationError"; break;
    case PollResponse::CapacitanceError:return "PollResponse::CapacitanceError"; break;
    case PollResponse::OperationError:return "PollResponse::OperationError"; break;
    case PollResponse::LengthError: return "PollResponse::LengthError"; break;
    case PollResponse::UnrecognisedBarcode: return "PollResponse::UnrecognisedBarcode"; break;
    case PollResponse::UvOptic: return "PollResponse::UvOptic"; break;
    case PollResponse::IncorrectNumberOfCharactersInBarcode: return "PollResponse::IncorrectNumberOfCharactersInBarcode"; break;
    case PollResponse::UnknownBarcodeStartSequence:  return "PollResponse::UnknownBarcodeStartSequence"; break;
    case PollResponse::UnknownBarcodeStopSequence: return "PollResponse::UnknownBarcodeStopSequence"; break;

    default:
        return QString("PollResponse::RejectReason: Unknown value: %1 ").arg(QString(QByteArray((char *)&reason,1).toHex()));
        break;
    }
}

QDebug operator<<(QDebug debug, PollResponse::Status status)
{
    debug << toString(status);
    return debug;
}

QString toString(PollResponse::Status status)

{
    switch (status) {
    case PollResponse::PowerUp:
        return "PollResponse::PowerUp";
        break;
    case PollResponse::PowerUpWithBillinValidator:
        return "PollResponse::PowerUpWithBillinValidator";
        break;
    case PollResponse::PowerUpWithBillinStacker:
        return "PollResponse::PowerUpWithBillinStacker";
        break;
    case PollResponse::Initilize:
        return "PollResponse::Initilize";
        break;
    case PollResponse::Idling:
        return "PollResponse::Idling";
        break;
    case PollResponse::Accepting:
        return "PollResponse::Accepting";
        break;
    case PollResponse::Stacking:
        return "PollResponse::Stacking";
        break;
    case PollResponse::Returning:
        return "PollResponse::Returning";
        break;
    case PollResponse::Disabled:
        return "PollResponse::Disabled";
        break;
    case PollResponse::Holding:
        return "PollResponse::Holding";
        break;
    case PollResponse::Busy:
        return "PollResponse::Busy";
        break;
    case PollResponse::Rejecting:
        return "PollResponse::Rejecting";
        break;
    case PollResponse::DropCassetteFull:
        return "PollResponse::DropCassetteFull";
        break;
    case PollResponse::DropCassetteOutOfPosition:
        return "PollResponse::DropCassetteOutOfPosition";
        break;
    case PollResponse::ValidatorJammed:
        return "PollResponse::ValidatorJammed";
        break;
    case PollResponse::DropCassetteJammed:
        return "PollResponse::DropCassetteJammed";
        break;
    case PollResponse::Cheated:
        return "PollResponse::Cheated";
        break;
    case PollResponse::Pause:
        return "PollResponse::Pause";
        break;
    case PollResponse::GenericFailure:
        return "PollResponse::GenericFailure";
        break;
    case PollResponse::EscrowPosition:
        return "PollResponse::EscrowPosition";
        break;
    case PollResponse::BillStacked:
        return "PollResponse::BillStacked";
        break;
    case PollResponse::BillReturned:
        return "PollResponse::BillReturned";
        break;

    default:
        return QString("PollResponse::Status: Unknown value: %1 ").arg(QString(QByteArray((char *)&status,1).toHex()));
        break;
    }
}
QString toString(PollResponse::GenericFailureReason failure)

{
    switch (failure) {
    case PollResponse::StackMotorFailure:
        return "PollResponse::StackMotorFailure";
        break;
    case PollResponse::TransportMotorSpeedFailure:
        return "PollResponse::TransportMotorSpeedFailure";
        break;
    case PollResponse::TransportMotorFailure:
        return "PollResponse::TransportMotorFailure";
        break;
    case PollResponse::AligningMotorFailure:
        return "PollResponse::AligningMotorFailure";
        break;
    case PollResponse::InitialCassetteStatusFailure:
        return "PollResponse::InitialCassetteStatusFailure";
        break;
    case PollResponse::OpticalCanalFailure:
        return "PollResponse::OpticalCanalFailure";
        break;
    case PollResponse::MagenaticCanalFailure:
        return "PollResponse::MagenaticCanalFailure";
        break;
    case PollResponse::CapacitanceCanalFailure:
        return "PollResponse::CapacitanceCanalFailure";
        break;

    default:
        return QString("PollResponse::GenericFailureReason Unknown value: %1 ").arg(QString(QByteArray((char *)&failure,1).toHex()));
    }
}


QDebug operator<<(QDebug debug, PollResponse::GenericFailureReason failure)
{
   debug << toString(failure);
   return debug;
}
