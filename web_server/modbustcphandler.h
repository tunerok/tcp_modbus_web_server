#ifndef MODBUSTCPHANDLER_H
#define MODBUSTCPHANDLER_H

#include <QSettings>
#include <QThread>
#include "modbusdatacontroller.h"
#include "modbustcpaddresses.h"


#ifdef MODBUS_MASTER
    #define MODBUS_RETRY_CONNECT_CNTR 2
    #include "mbport.h"
    #include "mbm.h"
    #include "common/mbportlayer.h"
#endif

#ifdef MODBUS_SLAVE
    #include "stdafx.h"
    #include "mb.h"
    #include "mbport.h"
#endif


class ModbusTCPHandler : public QThread
{
private:
    int _isMaster;
#ifdef MODBUS_MASTER
    QString _slavePort;
    QString _slaveIp;
    static int _slave_cntr;
    int _slave_num;
    xMBHandle    xMBMMaster;
    int _is_first_run;
#endif
#ifdef MODBUS_SLAVE
    int _slavePort;
#endif
public:
    ModbusTCPHandler(const QSettings *settings);
#ifdef MODBUS_MASTER
    static QMutex mutex;
    ModbusTCPHandler(const QSettings *settings, QString port, QString ip);
    static eMBErrorCode readRegisters(xMBMHandle xHdl, UCHAR ucSlaveAddress, USHORT usRegStartAddress, UBYTE ubNRegs, USHORT arusBufferOut[], int is_holding);
    static eMBErrorCode writeRegisters(xMBMHandle xHdl, UCHAR ucSlaveAddress, USHORT usRegStartAddress, UBYTE ubNRegs, const USHORT arusBufferIn[]);
    static eMBErrorCode writeSingleRegister(xMBMHandle xHdl, UCHAR ucSlaveAddress, USHORT usRegAddress, USHORT usValue);
#endif
    ~ModbusTCPHandler();
    void run() override;

};

#endif // MODBUSTCPHANDLER_H
