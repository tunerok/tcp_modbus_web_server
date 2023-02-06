#include "modbusdatacontroller.h"

ModbusDataController::ModbusDataController()
{

#ifdef MODBUS_SLAVE
    memset(this->_usRegHoldingBuf, 0, (REG_STRING_MAX_LEN-1)*2);
    memset(this->_usRegInputBuf, 0, (REG_STRING_MAX_LEN-1)*2);
#endif
#ifdef MODBUS_MASTER
    std::memset(this->_localdata, 0, 5);
    std::memset(this->_data, 0, 5);
#endif

}

#ifdef MODBUS_SLAVE
unsigned short ModbusDataController::getInputBuf(int iRegIndex){
    unsigned short tmp = 0;
    _mutex.lock();
    tmp = this->_usRegInputBuf[iRegIndex];
    _mutex.unlock();
    return tmp;
}

void ModbusDataController::setInputBuf(int iRegIndex, unsigned short Data){
    _mutex.lock();
    this->_usRegInputBuf[iRegIndex] = Data;
    _mutex.unlock();
}

unsigned short ModbusDataController::getHoldingBuf(int iRegIndex){
    unsigned short tmp = 0;
    _mutex.lock();
    tmp = this->_usRegHoldingBuf[iRegIndex];
    _mutex.unlock();
    return tmp;
}

void ModbusDataController::setHoldingBuf(int iRegIndex, unsigned short Data){
    _mutex.lock();
    this->_usRegHoldingBuf[iRegIndex] = Data;
    _mutex.unlock();
}


void ModbusDataController::getMasterMsg(QByteArray *msg){
    _mutex.lock();
    QByteArray tmp = QByteArray(reinterpret_cast<char*>(&this->_usRegHoldingBuf[REG_HOLDING_MASTER_DATA_START-REG_HOLDING_START+1]), REG_STRING_MAX_LEN-10);
    msg->append(tmp);
    _mutex.unlock();
}


void ModbusDataController::setSlaveMsg(QByteArray msg){

    _mutex.lock();
    memset(&this->_usRegInputBuf[REG_INPUT_SLAVE_DATA_START-REG_INPUT_START], 0, (REG_STRING_MAX_LEN-1)*2);
    std::memcpy((char*)&this->_usRegInputBuf[REG_INPUT_SLAVE_DATA_START-REG_INPUT_START], msg.constData(), msg.length());
    _mutex.unlock();
}
#endif


#ifdef MODBUS_MASTER
void ModbusDataController::updateInputStatus(unsigned short *Data, int slave){
    _mutex.lock();
    std::memcpy(this->_data[slave], Data, 5);
    _mutex.unlock();
}
bool ModbusDataController::checkHostDiscrete(int RegisterAddress, int slave){
    int retval = 0;
    switch (RegisterAddress) {
    case REG_INPUT_IS_MASTER_ICON_SHOW_ADDR:
        if (this->_data[slave][0])
            retval = 1;
        break;
    default:
        int tmp = RegisterAddress - 2000;
        if (tmp > 0)
            retval = this->_data[slave][tmp];
        break;
    }
    return retval;
}

void ModbusDataController::setLocalDiscrete(int RegisterAddress, int slave){
    _mutex.lock();
    if (RegisterAddress < 2000){
        this->_localdata[slave][0] = 1;
    }
    else {
        this->_localdata[slave][RegisterAddress - 2000] = 1;
    }
    _mutex.unlock();
}
bool ModbusDataController::getLocalDiscrete(int RegisterAddress, int slave){
    bool tmpdata = 0;
    _mutex.lock();
    if (RegisterAddress < 2000){
        tmpdata = this->_localdata[slave][0];
    }
    else {
        tmpdata = this->_localdata[slave][RegisterAddress - 2000];
    }
    _mutex.unlock();
    return tmpdata;
}

void ModbusDataController::clearLocalDiscrete(int RegisterAddress, int slave){
    _mutex.lock();
    if (RegisterAddress < 2000){
        this->_localdata[slave][0] = 0;
    }
    else {
        this->_localdata[slave][RegisterAddress - 2000] = 0;
    }
    _mutex.unlock();
}


bool ModbusDataController::putMasterStringData(QByteArray Data, int slave){
    _mutex.lock();
    this->_master_msg_data[slave].append(Data);
    _mutex.unlock();
    return true;
}


bool ModbusDataController::getMasterStringData(QByteArray *Data, int slave){
    _mutex.lock();
    Data->append(this->_master_msg_data[slave]);
    this->_master_msg_data[slave].clear();
    _mutex.unlock();
    return true;
}

bool ModbusDataController::bufMasterAvailable(int slave){
    if (this->_master_msg_data[slave].isEmpty())
        return 0;
    return true;
}

bool ModbusDataController::putSlaveStringData(QByteArray Data, int slave){
    _mutex.lock();
    this->_slave_msg_data[slave].append(Data);
    _mutex.unlock();
    return true;
}


bool ModbusDataController::getSlaveStringData(QByteArray *Data, int slave){
    _mutex.lock();
    Data->append(this->_slave_msg_data[slave]);
    this->_slave_msg_data[slave].clear();
    _mutex.unlock();
    return true;
}

bool ModbusDataController::bufSlaveAvailable(int slave){
    if (this->_slave_msg_data[slave].isEmpty())
        return 0;
    return 1;
}
#endif
