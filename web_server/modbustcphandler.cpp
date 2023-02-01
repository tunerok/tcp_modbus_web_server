#include "modbustcphandler.h"
#include <QDebug>

ModbusTCPHandler::ModbusTCPHandler()
{

#ifdef MODBUS_MASTER
    vMBPOtherDLLClose();
    vMBPOtherDLLInit();
    if (eMBMTCPInit( &this->xMBMMaster )){
        qDebug() << "TCP mbMaster init error";
    }

#endif

#ifdef MODBUS_SLAVE
    if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
    {
        qDebug() << "TCP mbSlave init error";
    }

#endif

}

ModbusTCPHandler::~ModbusTCPHandler(){
#ifdef MODBUS_MASTER
    eMBMClose( this->xMBMMaster );
    vMBPOtherDLLClose();
#endif

#ifdef MODBUS_SLAVE
    ( void )eMBClose(  );
#endif
}

void ModbusTCPHandler::run(){
#ifdef MODBUS_MASTER
    qDebug() << "Master thread runned";
    int i = 0;
    eMBErrorCode eStatus, eStatus2;
    eStatus = MB_EIO;
    while (1){
//    if (this->connected){

        while( i < 10 ){
            i++;
            if( MB_ENOERR != eStatus )
            {
             /* First disconnect - Function is safe even when not connected. */
            ( void )eMBMTCPDisconnect( this->xMBMMaster );
            eStatus = eMBMTCPConnect( this->xMBMMaster,  "127.0.0.1", 502 );
            }
            if( MB_ENOERR == eStatus )
            {
                if( MB_ENOERR != ( eStatus2 = eMBMWriteSingleRegister( this->xMBMMaster, 1, 2005, 55 ) ) )
                {
                    eStatus = eStatus2;
                }
            }
        }
        i = 0;
        QThread::sleep(1);
        qDebug() << "poll cycle: " << eStatus;
    }
#endif

#ifdef MODBUS_SLAVE
    if( eMBEnable(  ) == MB_ENOERR )
    {
        do
        {
            if( eMBPoll(  ) != MB_ENOERR )
                break;
        }
        while(1);
    }

    ( void )eMBDisable(  );

#endif
}
