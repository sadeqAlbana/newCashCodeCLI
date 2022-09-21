TEMPLATE = lib
QT += serialport
include(../intelhex/intelhex.pri)
SOURCES += \
        $$PWD/cashcode.cpp \
        $$PWD/cashcodecommand.cpp \
        $$PWD/ccnetexception.cpp \
        $$PWD/ccnetresponse.cpp \
        $$PWD/cserialport.cpp \
        $$PWD/getbilltableresponse.cpp \
        $$PWD/getstatusresponse.cpp \
        $$PWD/identificationresponse.cpp \
        $$PWD/pollresponse.cpp \
        $$PWD/utils.cpp


HEADERS += \
    $$PWD/cashcode.h \
    $$PWD/cashcodecommand.h \
    $$PWD/ccnetexception.h \
    $$PWD/ccnetresponse.h \
    $$PWD/cserialport.h \
    $$PWD/getbilltableresponse.h \
    $$PWD/getstatusresponse.h \
    $$PWD/identificationresponse.h \
    $$PWD/pollresponse.h \
    $$PWD/utils.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}

