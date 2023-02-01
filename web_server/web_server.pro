QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

LIBS += -lws2_32


#can be defined as MODBUS_MASTER or MODBUS_SLAVE
DEFINES += MODBUS_SLAVE

include(QtWebApp/QtWebApp/httpserver/httpserver.pri)


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        global.cpp \
        listdatacontroller.cpp \
        main.cpp \
        modbustcphandler.cpp \
        requestmapper.cpp \
        webrequestcontroller.cpp


contains(DEFINES,MODBUS_MASTER){
##include mbmaster
    INCLUDEPATH += $$PWD/modbusTCP/mbmaster/mbmaster/include
    INCLUDEPATH += $$PWD/modbusTCP/mbmaster/mbmaster/ascii
    INCLUDEPATH += $$PWD/modbusTCP/mbmaster/mbmaster/rtu
    INCLUDEPATH += $$PWD/modbusTCP/mbmaster/mbmaster/tcp
    INCLUDEPATH += $$PWD/modbusTCP/mbmaster/port

    SOURCES +=\
        modbusTCP/mbmaster/mbmaster/ascii/mbmascii.c \
        modbusTCP/mbmaster/mbmaster/common/mbutils.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfunccoils.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfunccustom1.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfuncdisc.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfuncfiles.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfuncholding.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfuncinput.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfuncraw.c \
        modbusTCP/mbmaster/mbmaster/functions/mbmfuncslaveid.c \
        modbusTCP/mbmaster/mbmaster/mbm.c \
        modbusTCP/mbmaster/mbmaster/rtu/mbmcrc.c \
        modbusTCP/mbmaster/mbmaster/rtu/mbmrtu.c \
        modbusTCP/mbmaster/mbmaster/tcp/mbmtcp.c \
        modbusTCP/mbmaster/mbmaster/udp/mbmudp.c \
        modbusTCP/mbmaster/port/mbportevent.c \
        modbusTCP/mbmaster/port/mbportother.c \
        modbusTCP/mbmaster/port/mbportserial.c \
        modbusTCP/mbmaster/port/mbporttcp.c \
        modbusTCP/mbmaster/port/mbporttimer.c \
        modbusTCP/mbmaster/port/mbportudp.c \
        modbusTCP/mbmaster/port/stdafx.cpp

    HEADERS += \
        modbusTCP/mbmaster/mbmaster/ascii/mbmascii.h \
        modbusTCP/mbmaster/mbmaster/include/common/mbframe.h \
        modbusTCP/mbmaster/mbmaster/include/common/mbportlayer.h \
        modbusTCP/mbmaster/mbmaster/include/common/mbtypes.h \
        modbusTCP/mbmaster/mbmaster/include/common/mbutils.h \
        modbusTCP/mbmaster/mbmaster/include/internal/mbmi.h \
        modbusTCP/mbmaster/mbmaster/include/internal/mbmiconfig.h \
        modbusTCP/mbmaster/mbmaster/include/internal/mbmiframe.h \
        modbusTCP/mbmaster/mbmaster/include/mbm.h \
        modbusTCP/mbmaster/mbmaster/rtu/mbmcrc.h \
        modbusTCP/mbmaster/mbmaster/rtu/mbmrtu.h \
        modbusTCP/mbmaster/mbmaster/tcp/mbmtcp.h \
        modbusTCP/mbmaster/mbmaster/udp/mbmudp.h \
        modbusTCP/mbmaster/port/mbmconfig.h \
        modbusTCP/mbmaster/port/mbport.h \
        modbusTCP/mbmaster/port/stdafx.h

}


contains(DEFINES,MODBUS_SLAVE){
##include mbslave
    INCLUDEPATH += $$PWD/modbusTCP/mbslave/modbus/include
    INCLUDEPATH += $$PWD/modbusTCP/mbslave/modbus/ascii
    INCLUDEPATH += $$PWD/modbusTCP/mbslave/modbus/rtu
    INCLUDEPATH += $$PWD/modbusTCP/mbslave/modbus/tcp
    INCLUDEPATH += $$PWD/modbusTCP/mbslave/port

    SOURCES +=\
        modbusTCP/mbslave/modbus/functions/mbfunccoils.c \
        modbusTCP/mbslave/modbus/functions/mbfuncdiag.c \
        modbusTCP/mbslave/modbus/functions/mbfuncdisc.c \
        modbusTCP/mbslave/modbus/functions/mbfuncholding.c \
        modbusTCP/mbslave/modbus/functions/mbfuncinput.c \
        modbusTCP/mbslave/modbus/functions/mbfuncother.c \
        modbusTCP/mbslave/modbus/functions/mbutils.c \
        modbusTCP/mbslave/modbus/mb.c \
        modbusTCP/mbslave/modbus/tcp/mbtcp.c \
        modbusTCP/mbslave/port/portevent.c \
        modbusTCP/mbslave/port/portother.c \
        modbusTCP/mbslave/port/porttcp.c \
        modbusTCP/mbslave/port/stdafx.cpp

    HEADERS += \
        modbusTCP/mbslave/modbus/include/mb.h \
        modbusTCP/mbslave/modbus/include/mbconfig.h \
        modbusTCP/mbslave/modbus/include/mbframe.h \
        modbusTCP/mbslave/modbus/include/mbfunc.h \
        modbusTCP/mbslave/modbus/include/mbport.h \
        modbusTCP/mbslave/modbus/include/mbproto.h \
        modbusTCP/mbslave/modbus/include/mbutils.h \
        modbusTCP/mbslave/modbus/tcp/mbtcp.h \
        modbusTCP/mbslave/port/port.h \
        modbusTCP/mbslave/port/stdafx.h

}

OTHER_FILES += etc/webapp1.ini
OTHER_FILES += etc/index/index_master.html
# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

HEADERS += \
    global.h \
    listdatacontroller.h \
    modbustcphandler.h \
    requestmapper.h \
    webrequestcontroller.h


DISTFILES += \
    modbusTCP/mbmaster/Changelog.txt \
    modbusTCP/mbmaster/md5sum.txt



