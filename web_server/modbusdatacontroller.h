#ifndef MODBUSDATACONTROLLER_H
#define MODBUSDATACONTROLLER_H

#include <QMutex>
#include <QByteArray>
#include <cstring>

#include "modbustcpaddresses.h"

class ModbusDataController
{

    QByteArray _master_msg_data[2];
    QByteArray _slave_msg_data[2];

    unsigned short _data[2][5];
    unsigned short _localdata[2][5];
    ModbusDataController();

public:
    QMutex _mutex;
    static ModbusDataController& getInstance(){
            static ModbusDataController theInstance;
            return theInstance;
        }

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

};

#endif // MODBUSDATACONTROLLER_H
