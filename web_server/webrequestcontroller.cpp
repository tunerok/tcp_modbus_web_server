#include "webrequestcontroller.h"

WebRequestController::WebRequestController(QObject* parent): HttpRequestHandler(parent)
{
    counter_data = 0;
}

void WebRequestController::service(HttpRequest &request, HttpResponse &response) {
    if (request.getMethod() == "POST")
    {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(request.getBody());
        QJsonObject jsonObject = jsonResponse.object();

        int request_type = jsonObject["request_type"].toInt();
        int slave_number = jsonObject["slave_number"].toInt();

        switch (request_type) {
        case 1:
        {
            QByteArray data = jsonObject["slave_data"].toString().toUtf8();
            //send data via modbus
            break;
        }
        case 2:
        {
            //change color
            break;
        }

        default:
            break;
        }
    }
    else{
        counter_data += 1;
        char tmp[100];
  //      response.setHeader("Content-Type", "text/html; charset=UTF-8");
   //     response.write("<html><body>");
        sprintf(tmp, "Request number: %d", counter_data);
        response.write(tmp, true);
   //     response.write("</body></header>",true);
    }





    //QByteArray *data = new QByteArray;



    //response.write("Hello World",true);
}
