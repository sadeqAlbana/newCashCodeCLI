#ifndef CASHCODECOMMAND_H
#define CASHCODECOMMAND_H

/*
format:
SYNC: 1 byte Message transmission start code [02H], fixed
ADR : 1 byte Peripheral address
LNG : 1 byte* Data length (Total number of bytes including SYNC and CRC)
CMD : 1 byte Command
DATA 0 to 250 bytes Data necessary for command (omitted if not required by CMD)
CRC: 2 bytes Check code by CRC method, LSB first
Object section to be from and including SYNC to end of DATA
(Initial value = 0)

* if a package cannot be fitted into 250-byte frame a wider frame may be used by setting LNG to 0; the actual packet
length is inserted into DATA block bytes 0 and 1 if CMD (if present in the frame) does not require subcommand,
otherwise in DATA block bytes 1 and 2; two-byte LNG always follows MSB first.



*/

/*

there are four message types

1- command transmission (Controller -> Peripheral)
2- ACK response (Peripheral <-> Controller)
3- NAK response (Peripheral -> Controller)
4- Response message (Peripheral -> Controller)
5- ILLEGAL command reponse message (Peripheral -> Controller) DATA=30H

*/

/*
Currently t free is defined as 10 mS of Bus silence, but for reliable operation of future multi-device
buses the recommended value of t free is 20mS
*/



class CashCodeCommand
{
public:
    CashCodeCommand();
};




#endif // CASHCODECOMMAND_H
