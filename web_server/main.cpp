#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QString>

#include <QTimer>
#include <QObject>

#include "httplistener.h"
#include "httprequesthandler.h"
#include "requestmapper.h"
#include "global.h"
#include "modbustcphandler.h"

ModbusTCPHandler* modbusTCPHandler_1;
#ifdef MODBUS_MASTER
ModbusTCPHandler* modbusTCPHandler_2;
#endif


//Search the configuration file.
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

    //Session store
    QSettings* sessionSettings = new QSettings(configFileName,QSettings::IniFormat,&app);
    sessionSettings->beginGroup("sessions");
    sessionStore = new HttpSessionStore(sessionSettings,&app);

    //Static file controller
    QSettings* fileSettings = new QSettings(configFileName,QSettings::IniFormat,&app);
    fileSettings->beginGroup("files");
    staticFileController = new StaticFileController(fileSettings,&app);

    //Modbus controller
    QSettings* modbusSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    modbusSettings->beginGroup("modbustcp");

#ifdef MODBUS_MASTER

    //If this is master then read slave count from .ini file and start threads
    vMBPOtherDLLClose();
    vMBPOtherDLLInit();
    QStringList sIpList, sPortList;
    QString slavesHostIps = modbusSettings->value("slavesHostIps","127.0.0.1 127.0.0.1").toString();
    QString slavesHostPorts = modbusSettings->value("slavesHostPorts","502 502").toString();
    sIpList.append(slavesHostIps.split(" "));
    sPortList.append(slavesHostPorts.split(" "));


    QString tmp_port = sPortList.at(0);
    QString tmp_ip = sIpList.at(0);
    modbusTCPHandler_1 = new  ModbusTCPHandler(modbusSettings, tmp_port, tmp_ip);
    modbusTCPHandler_1->start();
    if (modbusSettings->value("slaveCount","1").toInt() > 1){
        tmp_port = sPortList.at(1);
        tmp_ip = sIpList.at(1);
        modbusTCPHandler_2 = new  ModbusTCPHandler(modbusSettings, tmp_port, tmp_ip);
        modbusTCPHandler_2->start();
    }
#else
    //Run Modbus thread
    modbusTCPHandler_1 = new  ModbusTCPHandler(modbusSettings);
    modbusTCPHandler_1->start();
#endif


    //HTPP listener controller
    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings, new RequestMapper(&app),&app);

    return app.exec();
}


