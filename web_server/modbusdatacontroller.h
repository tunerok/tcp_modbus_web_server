#ifndef MODBUSDATACONTROLLER_H
#define MODBUSDATACONTROLLER_H

#ifdef MODBUS_SLAVE
#include <QString>
#endif

#include <QMutex>
#include <QByteArray>
#include <cstring>

#include "modbustcpaddresses.h"

class ModbusDataController
{
#ifdef MODBUS_SLAVE
    unsigned short _usRegInputBuf[REG_INPUT_NREGS];
    unsigned short _usRegHoldingBuf[REG_HOLDING_NREGS];
#endif
#ifdef MODBUS_MASTER
    QByteArray _master_msg_data[2];
    QByteArray _slave_msg_data[2];

    unsigned short _data[2][5];
    unsigned short _localdata[2][5];

#endif
    ModbusDataController();

public:
    QMutex _mutex;
    static ModbusDataController& getInstance(){
            static ModbusDataController theInstance;
            return theInstance;
        }

#ifdef MODBUS_SLAVE
    void setInputBuf(int iRegIndex, unsigned short Data);
    unsigned short getInputBuf(int iRegIndex);
    unsigned short getHoldingBuf(int iRegIndex);
    void setHoldingBuf(int iRegIndex, unsigned short Data);

    void getMasterMsg(QByteArray *msg);
    void setSlaveMsg(QByteArray msg);
#endif

#ifdef MODBUS_MASTER
    void updateInputStatus(unsigned short *Data, int slave);
    bool checkHostDiscrete(int RegisterAddress, int slave);

    void setLocalDiscrete(int RegisterAddress, int slave);
    bool getLocalDiscrete(int RegisterAddress, int slave);
    void clearLocalDiscrete(int RegisterAddress, int slave);


    bool putMasterStringData(QByteArray Data, int slave);
    bool getMasterStringData(QByteArray *Data, int slave);
    bool bufMasterAvailable(int slave);

    bool putSlaveStringData(QByteArray Data, int slave);
    bool getSlaveStringData(QByteArray *Data, int slave);
    bool bufSlaveAvailable(int slave);
#endif
};

#endif // MODBUSDATACONTROLLER_H
