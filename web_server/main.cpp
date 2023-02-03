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

    return app.exec();
}


