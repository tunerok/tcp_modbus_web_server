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

    int slave_number = 0;

    while (1){
//    if (this->connected){
        while( slave_number < 1 ){

            if( MB_ENOERR != eStatus )
            {
             /* First disconnect - Function is safe even when not connected. */
            ( void )eMBMTCPDisconnect( this->xMBMMaster );
            eStatus = eMBMTCPConnect( this->xMBMMaster,  "127.0.0.1", 502 );
            }
            if( MB_ENOERR == eStatus )
            {
                unsigned short usNRegs[5] = {0};
                if( MB_ENOERR != ( eStatus2 = eMBMReadHoldingRegisters( xMBMMaster, 1, 2000, 4,  &usNRegs[1]) ) )
                {
                    eStatus = eStatus2;
                }
                if( MB_ENOERR != ( eStatus2 = eMBMReadInputRegisters( xMBMMaster, 1, 1000, 1,  usNRegs) ) )
                {
                    eStatus = eStatus2;
                }
                ModbusDataController& DataController = ModbusDataController::getInstance();
                //parse data
                DataController.updateInputStatus(usNRegs, slave_number);

    //                //check slave input
                if (!DataController.checkHostDiscrete(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, slave_number)){
                    //DataController.
                    //READ NEW DATA FROM SLAVE
                    unsigned short temp_string[REG_STRING_MAX_LEN+1] = {0};
                    if( MB_ENOERR != ( eStatus2 = eMBMReadInputRegisters( xMBMMaster, 1, REG_INPUT_SLAVE_DATA_START, REG_STRING_MAX_LEN-10, temp_string ) ) )
                    {
                        eStatus = eStatus2;
                    }
                    std::string str( (char*)temp_string, (REG_STRING_MAX_LEN-10)*2);
                    QString qstr = QString::fromStdString(str);
                    DataController.putSlaveStringData(qstr.toUtf8(), slave_number);
                    qDebug() << qstr;

    //                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, 0 );
                }
                if(DataController.bufMasterAvailable(slave_number)){
                    QByteArray Data;
                    DataController.getMasterStringData(&Data, slave_number);
                    //ПРЕОБРАЗОВАТЬ И ОТПРАВИТЬ
                }

    //                //check icon state for master from slave
    //                if (DataController.checkHostDiscrete(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR)){

    //                    //icon.state = show
    //                }else {
    //                    //icon.state = hide
    //                }

                if (DataController.getLocalDiscrete(REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR, slave_number))
                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR-1, 1 );
                else
                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR-1, 0 );

                if (DataController.getLocalDiscrete(REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR, slave_number))
                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR-1, 1 );
                else
                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR-1, 0 );


                if(DataController.getLocalDiscrete(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, slave_number))
                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR-1, 1 );
                else
                    eMBMWriteSingleRegister( this->xMBMMaster, 1, REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR-1, 0 );

                qDebug() << "Registers data " << usNRegs[0] << usNRegs[1] << usNRegs[2] << usNRegs[3] << usNRegs[4];
    //                if( MB_ENOERR != ( eStatus2 = eMBMWriteSingleRegister( this->xMBMMaster, 1, 2005, 55 ) ) )
    //                {
    //                    eStatus = eStatus2;
    //                }

            }
            slave_number++;
        }
        slave_number = 0;

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
