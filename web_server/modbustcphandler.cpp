#include "modbustcphandler.h"
#include <QDebug>

ModbusTCPHandler::ModbusTCPHandler()
{

    //xMBHandle    xMBMMaster;

#ifdef MODBUS_MASTER
    vMBPOtherDLLClose();
    vMBPOtherDLLInit();
    eMBMTCPInit( &this->xMBMMaster );
//    eMBMTCPConnect( this->xMBMMaster, "127.0.0.1", 502 );
#endif

#ifdef MODBUS_SLAVE
    if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
    {
        _ftprintf( stderr, _T( "%s: can't initialize modbus stack!\r\n" ), PROG );
        iExitCode = EXIT_FAILURE;
    }
    InitializeCriticalSection( &hPollLock );
    eSetPollingThreadState( STOPPED );
    bCreatePollingThread(  );

#endif

}

ModbusTCPHandler::~ModbusTCPHandler(){
    eMBMClose( this->xMBMMaster );
    vMBPOtherDLLClose();
}

void ModbusTCPHandler::run(){
    qDebug() << "runned";
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
//    }
//    else{
//        vMBPOtherDLLInit();
//        eMBMTCPInit( &xMBMMaster );
//        eMBMTCPConnect( xMBMMaster, CLIENT_HOSTNAME, CLIENT_PORT );
//    }


//    eMBErrorCode eStatus, eStatus2;
//    xMBHandle    xMBMMaster;
//    USHORT       usNRegs[5];
//    USHORT       usRegCnt = 0;
//    UBYTE        ubIdx;
//    int          iPolls = 100;

//    vMBPOtherDLLInit();

//    if( MB_ENOERR == ( eStatus = eMBMTCPInit( &xMBMMaster ) ) )
//    {
//        eStatus = MB_EIO;
//        while( iPolls-- > 0 )
//        {
//            if( MB_ENOERR != eStatus )
//            {
//                /* First disconnect - Function is safe even when not connected. */
//                ( void )eMBMTCPDisconnect( xMBMMaster );
//                eStatus = eMBMTCPConnect( xMBMMaster, "127.0.0.1", 502 );
//            }
//            if( MB_ENOERR == eStatus )
//            {
//                    if( MB_ENOERR != ( eStatus2 = eMBMWriteSingleRegister( xMBMMaster, 1, 2005, 55 ) ) )
//                    {
//                        eStatus = eStatus2;
//                    }
//            }
//            _ftprintf( stderr, _T( "poll cycle: %s\n" ), eStatus == MB_ENOERR ? _T( "okay" ) : _T( "failed" ) );
//        }
//    }
//    else
//    {
//        _ftprintf( stderr, _T( "TCP init failed with error = %d\n" ), eStatus );
//    }
//    if( MB_ENOERR != ( eStatus = eMBMClose( xMBMMaster ) ) )
//    {
//        MBP_ASSERT( 0 );
//    }

//    vMBPOtherDLLClose();





}
