#include "modbustcphandler.h"
#include <QDebug>

ModbusTCPHandler::ModbusTCPHandler(const QSettings *settings)
{

    //Settings loading
    this->_isMaster = settings->value("isMaster","0").toInt();
    qDebug() << this->_isMaster;

#ifdef MODBUS_SLAVE
    //settings loading slave
    this->_slavePort = settings->value("slavePort","502").toInt();
#endif
}
#ifdef MODBUS_MASTER

int ModbusTCPHandler::_slave_cntr = 0;

ModbusTCPHandler::ModbusTCPHandler(const QSettings *settings, QString port, QString ip)
{

    //Settings loading
    this->_isMaster = settings->value("isMaster","1").toInt();
    qDebug() << this->_isMaster;
    this->_slaveIp = ip;
    this->_slavePort = port;

    this->_slave_num = this->_slave_cntr;
    this->_slave_cntr++;

    //Init
    this->_is_first_run = 1;
    vMBPOtherDLLInit();

}
#endif

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
    static int connect_reboot_cntr = 0;
    int i = 0;
    eMBErrorCode eStatus, eStatus2;
    eStatus = MB_EIO;

    int slave_number = this->_slave_num;


    while (1){

        if(this->_is_first_run == 0){
            eMBMTCPDisconnect( xMBMMaster );
            eMBMClose( xMBMMaster );

        }
        else{
            this->_is_first_run = 0;
        }

        if (eMBMTCPInit( &this->xMBMMaster )){
            qDebug() << "TCP mbMaster init error";

        }
        else{
            connect_reboot_cntr = 0;
        }

        //If has several errors - try reconnect
         while (connect_reboot_cntr < MODBUS_RETRY_CONNECT_CNTR){

            if( MB_ENOERR != eStatus )
            {
             /* First disconnect - Function is safe even when not connected. */
            ( void )eMBMTCPDisconnect( this->xMBMMaster );
            eStatus = eMBMTCPConnect( this->xMBMMaster,  this->_slaveIp.toStdString().c_str(),  this->_slavePort.toInt() );

            }
            if( MB_ENOERR == eStatus )
            {
                unsigned short usNRegs[5] = {0};
                if( MB_ENOERR != ( eStatus2 = readRegisters( xMBMMaster, this->_slave_num, 2000, 4,  &usNRegs[1], 1) ) )
                {
                    eStatus = eStatus2;
                }
                if( MB_ENOERR != ( eStatus2 = readRegisters( xMBMMaster, this->_slave_num, 1000, 1,  usNRegs, 0) ) )
                {
                    eStatus = eStatus2;
                }
                ModbusDataController& DataController = ModbusDataController::getInstance();
                //Update local data and parse
                DataController.updateInputStatus(usNRegs, slave_number);

              //check slave input
                if (DataController.checkHostDiscrete(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, slave_number)){
                    //Read data from slave
                    unsigned short temp_string[REG_STRING_MAX_LEN+1] = {0};
                    if( MB_ENOERR != ( eStatus2 = readRegisters( xMBMMaster, this->_slave_num, REG_INPUT_SLAVE_DATA_START-1, REG_STRING_MAX_LEN-10, temp_string , 0) ) )
                    {
                        eStatus = eStatus2;
                    }
                    QByteArray tmp = QByteArray(reinterpret_cast<char*>(temp_string), REG_STRING_MAX_LEN-10);
                    DataController.putSlaveStringData(tmp, slave_number);
                    DataController.clearLocalDiscrete(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, slave_number);
                    qDebug() << tmp;
                }
                if(DataController.bufMasterAvailable(slave_number)){
                    //Write data to slave
                    QByteArray Data;
                    unsigned short temp_string[REG_STRING_MAX_LEN+1] = {0};
                    DataController.getMasterStringData(&Data, slave_number);
                    std::memcpy((char*)&temp_string, Data.constData(), Data.length());
                    if( MB_ENOERR != ( eStatus2 = writeRegisters( xMBMMaster, this->_slave_num, REG_HOLDING_MASTER_DATA_START, Data.length()*2, temp_string ) ) )
                    {
                        eStatus = eStatus2;
                    }
                    DataController.setLocalDiscrete(REG_HOLDING_IS_MASTER_DATA_NEW_ADDR, slave_number);

                }

                //Update slave registers from local storage
                if (DataController.getLocalDiscrete(REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR, slave_number))
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR-1, 1 );
                else
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR-1, 0 );

                if (DataController.getLocalDiscrete(REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR, slave_number))
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR-1, 1 );
                else
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR-1, 0 );

                if(DataController.getLocalDiscrete(REG_HOLDING_IS_MASTER_DATA_NEW_ADDR, slave_number)){
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_MASTER_DATA_NEW_ADDR-1, 1 );
                    DataController.clearLocalDiscrete(REG_HOLDING_IS_MASTER_DATA_NEW_ADDR, slave_number);
                }

                if(DataController.getLocalDiscrete(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, slave_number))
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR-1, 1 );
                else
                    writeSingleRegister( this->xMBMMaster, this->_slave_num, REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR-1, 0 );


                qDebug() << "Registers data " << usNRegs[0] << usNRegs[1] << usNRegs[2] << usNRegs[3] << usNRegs[4];


            }
                qDebug() << "Thread id:  " << this->_slave_num;
                qDebug() << "Slave port:  " << this->_slavePort;
                qDebug() << "Slave ip:  " << this->_slaveIp;

            QThread::msleep(500);
            qDebug() << "poll cycle: " << eStatus;
            if (eStatus != MB_ENOERR){
                connect_reboot_cntr++;
            }
            else{
                connect_reboot_cntr = 0;
            }
        }
   }
#endif

#ifdef MODBUS_SLAVE
    while(1){
        if( eMBTCPInit( this->_slavePort ) != MB_ENOERR )
        {
            qDebug() << "Slave thread: Start Error";
        }
        else{
            qDebug() << "Slave thread: started";
            if( eMBEnable(  ) == MB_ENOERR )
            {
                do
                {
                    if( eMBPoll(  ) != MB_ENOERR ){
                        qDebug() << "Slave thread: Error";
                        break;
                    }
                }
                while(1);
            }

            ( void )eMBDisable(  );
            ( void )eMBClose(  );
        }
    }

#endif
}


#ifdef MODBUS_MASTER
//Static mutex
QMutex ModbusTCPHandler::mutex;

//Static functions for interface with blocking feature
eMBErrorCode ModbusTCPHandler::readRegisters(xMBMHandle xHdl, UCHAR ucSlaveAddress, USHORT usRegStartAddress, UBYTE ubNRegs, USHORT arusBufferOut[], int is_holding){
    eMBErrorCode status = MB_ENOERR;
    ModbusTCPHandler::mutex.lock();
    if (is_holding){
       status = eMBMReadHoldingRegisters( xHdl, ucSlaveAddress, usRegStartAddress, ubNRegs, arusBufferOut);
    }
    else {
        status = eMBMReadInputRegisters(  xHdl,  ucSlaveAddress,  usRegStartAddress,  ubNRegs,  arusBufferOut);
    }
    ModbusTCPHandler::mutex.unlock();
    return status;
}

eMBErrorCode ModbusTCPHandler::writeRegisters(xMBMHandle xHdl, UCHAR ucSlaveAddress, USHORT usRegStartAddress, UBYTE ubNRegs, const USHORT arusBufferIn[]){
    eMBErrorCode status = MB_ENOERR;
    ModbusTCPHandler::mutex.lock();
    status = eMBMWriteMultipleRegisters( xHdl,  ucSlaveAddress,  usRegStartAddress,  ubNRegs,arusBufferIn );
    ModbusTCPHandler::mutex.unlock();
    return status;

}

eMBErrorCode ModbusTCPHandler::writeSingleRegister(xMBMHandle xHdl, UCHAR ucSlaveAddress, USHORT usRegAddress, USHORT usValue){
    eMBErrorCode status = MB_ENOERR;
    ModbusTCPHandler::mutex.lock();
    status = eMBMWriteSingleRegister( xHdl,  ucSlaveAddress,  usRegAddress,  usValue);
    ModbusTCPHandler::mutex.unlock();
    return status;
}
#endif
