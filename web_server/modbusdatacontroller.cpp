#include "modbusdatacontroller.h"

ModbusDataController::ModbusDataController()
{
    std::memset(this->_localdata, 0, 5);
}

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
}


bool ModbusDataController::getMasterStringData(QByteArray *Data, int slave){
    _mutex.lock();
    Data->append(this->_master_msg_data[slave]);
    this->_master_msg_data[slave].clear();
//    if (this->_master_msg_data[slave].length() > REG_STRING_MAX_LEN-10){
//        std::memcpy(Data, this->_master_msg_data[slave].constData(), REG_STRING_MAX_LEN-10);
//        this->_master_msg_data[slave].remove(0, REG_STRING_MAX_LEN-10);
//    }
//    else {
//        std::memcpy(Data, this->_master_msg_data[slave].constData(), this->_master_msg_data[slave].length());
//        this->_master_msg_data[slave].clear();
//    }
    _mutex.unlock();
}

bool ModbusDataController::bufMasterAvailable(int slave){
    if (this->_master_msg_data[slave].isEmpty())
        return 0;
    return 1;
}

bool ModbusDataController::putSlaveStringData(QByteArray Data, int slave){
    _mutex.lock();
    this->_slave_msg_data[slave].append(Data);
    _mutex.unlock();
}


bool ModbusDataController::getSlaveStringData(QByteArray *Data, int slave){
    _mutex.lock();
    Data->append(this->_slave_msg_data[slave]);
    this->_slave_msg_data[slave].clear();

//    if (this->_slave_msg_data[slave].length() > REG_STRING_MAX_LEN-10){
//        std::memcpy(Data, this->_slave_msg_data[slave].constData(), REG_STRING_MAX_LEN-10);
//        this->_slave_msg_data[slave].remove(0, REG_STRING_MAX_LEN-10);
//    }
//    else {
//        std::memcpy(Data, this->_slave_msg_data[slave].constData(), this->_slave_msg_data[slave].length());
//        this->_slave_msg_data[slave].clear();
//    }
    _mutex.unlock();
}

bool ModbusDataController::bufSlaveAvailable(int slave){
    if (this->_slave_msg_data[slave].isEmpty())
        return 0;
    return 1;
}
