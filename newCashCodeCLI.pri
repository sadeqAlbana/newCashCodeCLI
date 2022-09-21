INCLUDEPATH += $$PWD/src

include(intelhex/intelhex.pri)
SOURCES += \
        $$PWD/src/cashcode.cpp \
        $$PWD/src/cashcodecommand.cpp \
        $$PWD/src/ccnetexception.cpp \
        $$PWD/src/ccnetresponse.cpp \
        $$PWD/src/cserialport.cpp \
        $$PWD/src/getbilltableresponse.cpp \
        $$PWD/src/getstatusresponse.cpp \
        $$PWD/src/identificationresponse.cpp \
        $$PWD/src/pollresponse.cpp \
        $$PWD/src/utils.cpp

HEADERS += \
    $$PWD/src/cashcode.h \
    $$PWD/src/cashcodecommand.h \
    $$PWD/src/ccnetexception.h \
    $$PWD/src/ccnetresponse.h \
    $$PWD/src/cserialport.h \
    $$PWD/src/getbilltableresponse.h \
    $$PWD/src/getstatusresponse.h \
    $$PWD/src/identificationresponse.h \
    $$PWD/src/pollresponse.h \
    $$PWD/src/utils.h

