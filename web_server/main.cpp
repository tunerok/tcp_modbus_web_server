#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>

#include <QTimer>
#include <QObject>

#include "httplistener.h"
#include "httprequesthandler.h"
#include "listdatacontroller.h"
#include "requestmapper.h"
#include "global.h"
#include "modbustcphandler.h"




#ifdef MODBUS_SLAVE

#define REG_INPUT_START 1000
#define REG_INPUT_IS_MASTER_ICON_SHOW_ADDR 1001
#define REG_INPUT_SLAVE_DATA_START 1002
#define REG_INPUT_NREGS 132

#define REG_HOLDING_START 2000
#define REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR 2001
#define REG_HOLDING_IS_MASTER_DATA_NEW_ADDR 2002
#define REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR 2003
#define REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR 2004
#define REG_HOLDING_MASTER_DATA_START 2005
#define REG_HOLDING_NREGS 135

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
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
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
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



/**
 * Search the configuration file.
 * Aborts the application if not found.
 * @return The valid filename
 */
QString searchConfigFile() {
    QString binDir=QCoreApplication::applicationDirPath();
    QString appName=QCoreApplication::applicationName();
    QString fileName("webapp1.ini");

    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc");
    searchList.append(binDir+"/../etc");
    searchList.append(binDir+"/../"+appName+"/etc");     // for development with shadow build (Linux)
    searchList.append(binDir+"/../../"+appName+"/etc");  // for development with shadow build (Windows)
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (QString dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
    return nullptr;
}


using namespace stefanfrings;



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QString configFileName=searchConfigFile();

    // Session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    sessionSettings->beginGroup("sessions");
    sessionStore=new HttpSessionStore(sessionSettings,&app);

    // Static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    fileSettings->beginGroup("files");
    staticFileController=new StaticFileController(fileSettings,&app);



//    QTimer *timer = new QTimer();
//    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(modbus->dataSync();));
//    timer->start(1000);


    QSettings* listenerSettings=new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");
    //new HttpListener(listenerSettings, new HttpRequestHandler(&app), &app);
    //new HttpListener(listenerSettings,new HelloWorldController(&app),&app);
    new HttpListener(listenerSettings, new RequestMapper(&app),&app);

    qDebug("Hello World");
    return app.exec();
}


