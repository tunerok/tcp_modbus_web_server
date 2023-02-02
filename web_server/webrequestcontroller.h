#ifndef WEBREQUESTCONTROLLER_H
#define WEBREQUESTCONTROLLER_H

#include "httprequesthandler.h"
#include "modbustcphandler.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

using namespace stefanfrings;

class WebRequestController : public HttpRequestHandler
{
    int counter_data;
    ModbusTCPHandler _sdmodbus;
public:
    WebRequestController(QObject* parent=0);
    void service(HttpRequest& request, HttpResponse& response);
};

#endif // WEBREQUESTCONTROLLER_H
