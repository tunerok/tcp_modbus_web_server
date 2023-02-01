#ifndef MODBUSTCPHANDLER_H
#define MODBUSTCPHANDLER_H

#include <QThread>

#ifdef MODBUS_MASTER
    /* ----------------------- Platform includes --------------------------------*/
    #include "mbport.h"

    /* ----------------------- Modbus includes ----------------------------------*/
    #include "mbm.h"
    #include "common/mbportlayer.h"
#endif

#ifdef MODBUS_SLAVE
    #include "stdafx.h"

    /* ----------------------- Modbus includes ----------------------------------*/
    #include "mb.h"
    #include "mbport.h"
#endif


class ModbusTCPHandler : public QThread
{
private:

public:
    xMBHandle    xMBMMaster;
    ModbusTCPHandler( );
    ~ModbusTCPHandler();

#ifdef MODBUS_MASTER
    void run() override;

#endif

#ifdef MODBUS_SLAVE
    BOOL bCreatePollingThread( void );
    DWORD WINAPI dwPollingThread( LPVOID lpParameter );
    enum ThreadState eGetPollingThreadState(  );
    void eSetPollingThreadState( enum ThreadState eNewState );

    eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );
    eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode );
    eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode );
    eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );
#endif
};

#endif // MODBUSTCPHANDLER_H
