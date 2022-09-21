cmake_minimum_required(VERSION 3.16)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)

add_library(SCCNET STATIC
    cashcode.h cashcode.cpp
    utils.h utils.cpp
    pollresponse.h pollresponse.cpp
    ccnetexception.h ccnetexception.cpp
    identificationresponse.h identificationresponse.cpp
    getbilltableresponse.h getbilltableresponse.cpp
    cserialport.h cserialport.cpp
    ccnetresponse.h ccnetresponse.cpp
    getstatusresponse.h getstatusresponse.cpp
    cashcodecommand.h cashcodecommand.cpp
)

target_link_libraries(SCCNET PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::SerialPort)

target_include_directories(SCCNET INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
