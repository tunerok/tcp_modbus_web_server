#include "webrequestcontroller.h"

WebRequestController::WebRequestController(QObject* parent): HttpRequestHandler(parent)
{
    counter_data = 0;
    this->_sdmodbus.start();
}

void WebRequestController::service(HttpRequest &request, HttpResponse &response) {
    ModbusDataController& DataController = ModbusDataController::getInstance();
    if (request.getMethod() == "POST")
    {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(request.getBody());
        QJsonObject jsonObject = jsonResponse.object();

        int request_type = jsonObject["request_type"].toInt();
        int slave_number = jsonObject["slave_number"].toInt();

        switch (request_type) {
        case 1:
        {
            //send string via modbus
            QByteArray data = jsonObject["slave_data"].toString().toUtf8();
            DataController.putMasterStringData(data, slave_number);
            DataController.setLocalDiscrete(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR, slave_number);
            break;
        }
        case 2:{
        {
            //change color
            int slave_color = jsonObject["slave_data"].toInt();
            if (slave_color)
                DataController.setLocalDiscrete(REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR, slave_number);
            else {
                DataController.clearLocalDiscrete(REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR, slave_number);
            }
            break;
            }
        }
        case 3:{
            //btn states
            int btn_state = jsonObject["slave_data"].toInt();
            if (btn_state)
                DataController.setLocalDiscrete(REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR, slave_number);
            else {
                DataController.clearLocalDiscrete(REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR, slave_number);
            }
            break;
        }

        default:
            break;
        }
    }else if (request.getMethod() == "GET") {

        QJsonObject object;

        object.insert("s1_icon_show_data", DataController.checkHostDiscrete(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR, 0));
        object.insert("s2_icon_show_data", DataController.checkHostDiscrete(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR, 1));

        if (DataController.bufSlaveAvailable(0)){
            QByteArray Data;
            DataController.getSlaveStringData(&Data, 0);
            QString DataAsString = QString(Data);
            object.insert("s1_msg_data", DataAsString);
        }
        else {
            object.insert("s1_msg_data", "");
        }
        QJsonDocument doc(object);
        QByteArray bytes = doc.toJson();
        response.write(bytes, true);

//        if(DataController.checkHostDiscrete(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR, 0)){
//            object.insert("icon_show_data_s1", 1);
//            //response.write("{\"data\":\"1\"}", false);
//        }else{
//           // response.write("{\"data\":\"0\"}", false);
//        }
//        if (DataController.bufSlaveAvailable(0)){
//            QByteArray Data;
//            DataController.getSlaveStringData(&Data, 0);
//            response.write(Data, true);
//        }

    }
//    else{
//        counter_data += 1;
//        char tmp[100];
//  //      response.setHeader("Content-Type", "text/html; charset=UTF-8");
//   //     response.write("<html><body>");
//        sprintf(tmp, "Request number: %d", counter_data);
//        response.write(tmp, true);
//   //     response.write("</body></header>",true);
//    }





    //QByteArray *data = new QByteArray;



    //response.write("Hello World",true);
}
