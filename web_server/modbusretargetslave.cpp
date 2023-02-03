#ifdef MODBUS_SLAVE

#include "modbustcphandler.h"


//#define REG_INPUT_START 1000
//#define REG_INPUT_IS_MASTER_ICON_SHOW_ADDR 1001
//#define REG_INPUT_SLAVE_DATA_START 1002
//#define REG_INPUT_NREGS 132

//#define REG_HOLDING_START 2000
//#define REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR 2001
//#define REG_HOLDING_IS_MASTER_DATA_NEW_ADDR 2002
//#define REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR 2003
//#define REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR 2004
//#define REG_HOLDING_MASTER_DATA_START 2005
//#define REG_HOLDING_NREGS 135

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
//static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
//static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];



eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    ModbusDataController& DataController = ModbusDataController::getInstance();
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            unsigned short tmp_data = DataController.getInputBuf(iRegIndex);
            *pucRegBuffer++ = ( unsigned char )( tmp_data >> 8 );
            *pucRegBuffer++ = ( unsigned char )( tmp_data & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    ModbusDataController& DataController = ModbusDataController::getInstance();
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                unsigned short tmp_data = DataController.getHoldingBuf(iRegIndex);
                *pucRegBuffer++ = ( UCHAR ) ( tmp_data >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( tmp_data & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                unsigned short tmp_data = 0;
                tmp_data = *pucRegBuffer++ << 8;
                tmp_data |= *pucRegBuffer++;
                DataController.setHoldingBuf(iRegIndex, tmp_data);
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
#endif
