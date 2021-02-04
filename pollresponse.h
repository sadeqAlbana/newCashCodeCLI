#ifndef POLLRESPONSE_H
#define POLLRESPONSE_H
#include "ccnetresponse.h"



class PollResponse : public CCNetResponse
{
public:
    enum Status : quint8{ //z1
        PowerUp = 0x10, //The state of the Bill Validator after power up
        PowerUpWithBillinValidator = 0x11, //Power up with bill in the Bill Validator. After a RESET command from the Controller Bill Validator returns the bill and continues initializing.
        PowerUpWithBillinStacker = 0x12, //Power Up with Bill in Stacker. Power up with bill in Stacker (Bill was transported too far to be returned). After the Bill Validator is reset and INITIALIZING is complete, status will immediately change to STACKED (81H) (Credit Recovery feature).
        Initilize = 0x13, //Bill Validator executes initialization after the RESET command from Controller.
        Idling = 0x14, //Bill Validator waits for an inserting of bill into its bill path.
        Accepting = 0x15, //Bill Validator executes scanning of a bill and determines its denomination.
        Stacking = 0x17, //Bill Validator transports a bill from Escrow position to drop cassette and remains in this state until the bill is stacked or jammed.
        Returning = 0x18, //Bill Validator transports a bill from Escrow position back to customer and remains in this state until customer removes the bill or the bill is jammed.
        Disabled = 0x19, //Bill Validator has been disabled by the Controller or just came out of initialization
        Holding = 0x1A, //The state, in which the bill is held in Escrow position after the HOLD command of the Controller.
        Busy    = 0x1B, //has Z2, Bill Validator cannot answer with a full-length message right now. On expiration of time YH, peripheral is accessible to polling. YH is expressed in multiple of 100 milliseconds.
        Rejecting   = 0x1C, //Generic rejecting code. Always followed by rejection reason byte.
        DropCassetteFull    =0x41,
        DropCassetteOutOfPosition    =0x42,
        ValidatorJammed    =0x43,
        DropCassetteJammed     =0x44,
        Cheated    =0x45,
        Pause = 0x46, // *Bill validator is deadlocked in “Pause” state; to resolve deadlock “Reset” command may be issued.
        GenericFailure    =0x47, //has Z2
        EscrowPosition = 0x80, //has Z2
        BillStacked = 0x81, // has Z2
        BillReturned = 0x82 // has Z2

    };

    /*
    * Bill validator is deadlocked in “Pause” state; to resolve deadlock “Reset” command may be issued.
    - YH =17H(23 10 ) corresponds to a barcode coupon.
    */

    enum  RejectReason : quint8{ //z2
        InsertionError = 0x60, //Rejecting due to Insertion -  Insertion error
        MagenaticError = 0x61, //Rejecting due to Magnetic - Dielectric error
        BillInTheHead = 0x62, //Rejecting due to Remained bill om head. - Previously inserted bill remains in head
        Multiplying = 0x63, //Rejecting due to Multiplying - Compensation error/multiplying factor error
        ConveyingError = 0x64, //Rejecting due to Conveying. Bill transport error
        IdentificationError = 0x65, //Rejecting due to Identification1. Identification error
        VerificationError = 0x66, //Rejecting due to Verification. Verification error
        OpticError = 0x67, //REJECTING - Rejecting due to Optic. Optic error.
        InhibitDenominationError    = 0x68, //Rejecting due to Inhibit. Return by “inhibit denomination” error
        CapacitanceError = 0x69, //Rejecting due to Capacity. Capacitance error
        OperationError = 0x6A, //Rejecting due to Operation. Operation error
        LengthError = 0x6C, //Rejecting due to Length. Length error
        UnrecognisedBarcode = 0x92, //Rejecting due to unrecognised barcode. Bill taken was treated as a barcode but no reliable data can be read from it.
        UvOptic = 0x6D, //Rejecting due to UV. Banknote UV properties do not meet the predefined criteria
        IncorrectNumberOfCharactersInBarcode = 0x93, //Rejecting due to incorrect number of characters in barcode. Barcode data was read (at list partially) but is inconsistent.
        UnknownBarcodeStartSequence = 0x94, //Barcode data was read (at list partially) but is inconsistent.
        UnknownBarcodeStopSequence = 0x95 //Barcode was not read as no synchronization was established.
    };

    enum  GenericFailureReason : quint8{
        StackMotorFailure = 0x50,
        TransportMotorSpeedFailure = 0x51,
        TransportMotorFailure = 0x52,
        AligningMotorFailure = 0x53,
        InitialCassetteStatusFailure = 0x54,
        OpticalCanalFailure = 0x55,
        MagenaticCanalFailure = 0x56,
        CapacitanceCanalFailure = 0x5F,


    };










public:
    PollResponse(const QByteArray &data);
    PollResponse(const CCNetResponse &other);

    PollResponse::Status status() const;
    PollResponse::GenericFailureReason failureReason() const;
    PollResponse::RejectReason rejectReason() const;

    int stackedBill() const;

    friend QDebug operator<<(QDebug debug, PollResponse::Status status );
    friend QDebug operator<<(QDebug debug, PollResponse::RejectReason reason);
    friend QDebug operator<<(QDebug debug, PollResponse::GenericFailureReason failure);






};

QString toString(PollResponse::Status status);
QString toString(PollResponse::RejectReason reason);
QString toString(PollResponse::GenericFailureReason failure);

QDebug operator<<(QDebug debug, PollResponse::Status status );
QDebug operator<<(QDebug debug, PollResponse::RejectReason reason);
QDebug operator<<(QDebug debug, PollResponse::GenericFailureReason failure);




#endif // POLLRESPONSE_H
