#include "webrequestcontroller.h"

WebRequestController::WebRequestController(QObject* parent): HttpRequestHandler(parent)
{

}
#ifdef MODBUS_MASTER
void WebRequestController::service(HttpRequest &request, HttpResponse &response) {

    //Get instance of DATA controller
    ModbusDataController& DataController = ModbusDataController::getInstance();

    if (request.getMethod() == "POST")
    {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(request.getBody());
        QJsonObject jsonObject = jsonResponse.object();

        int request_type = jsonObject["request_type"].toInt();
        int slave_number = jsonObject["slave_number"].toInt() - 1;

        switch (request_type) {
        case 1:
        {
            //Save string to local storage
            QByteArray data = jsonObject["slave_data"].toString().toUtf8();
            //Set READY flag
            DataController.putMasterStringData(data, slave_number);
            break;
        }
        case 2:{
        {
            //Change color
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
            //Btn states
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

        //JSON data prepare
        QJsonObject object;

        //Show current status of icons
        object.insert("s1_icon_show_data", DataController.checkHostDiscrete(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR, 0));
        object.insert("s2_icon_show_data", DataController.checkHostDiscrete(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR, 1));

        //Read data from buffer and show it
        if (DataController.bufSlaveAvailable(0)){
            QByteArray Data;
            DataController.getSlaveStringData(&Data, 0);
            QString DataAsString = QString(Data);
            object.insert("s1_msg_data", DataAsString);
        }
        else {
            object.insert("s1_msg_data", "");
        }
        if (DataController.bufSlaveAvailable(1)){
            QByteArray Data;
            DataController.getSlaveStringData(&Data, 1);
            QString DataAsString = QString(Data);
            object.insert("s2_msg_data", DataAsString);
        }
        else {
            object.insert("s2_msg_data", "");
        }

        //Serialize JSON to QByteArray and send it as answer
        QJsonDocument doc(object);
        QByteArray bytes = doc.toJson();
        response.write(bytes, true);
    }
}
#endif


#ifdef MODBUS_SLAVE
void WebRequestController::service(HttpRequest &request, HttpResponse &response) {
    ModbusDataController& DataController = ModbusDataController::getInstance();
    if (request.getMethod() == "POST")
    {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(request.getBody());
        QJsonObject jsonObject = jsonResponse.object();

        int request_type = jsonObject["request_type"].toInt();
        switch (request_type) {
        case 1:
        {
            //send string via modbus
            QByteArray data = jsonObject["slave_data"].toString().toUtf8();
            DataController.setSlaveMsg(data);
            DataController.setHoldingBuf(REG_HOLDING_IS_SLAVE_INPUT_NEW_ADDR - REG_HOLDING_START, 1);
            break;
        }
        case 2:{
        {
            //change color
            int slave_color = jsonObject["slave_data"].toInt();
            DataController.setInputBuf(REG_INPUT_IS_MASTER_ICON_SHOW_ADDR - REG_INPUT_START, slave_color);
            break;
            }
        }
        default:
            break;
        }
    }else if (request.getMethod() == "GET") {

        QJsonObject object;

        object.insert("s_icon_show_data", DataController.getHoldingBuf(REG_HOLDING_IS_SLAVE_ICON_SHOW_ADDR - REG_HOLDING_START));
        object.insert("s_msg_color", DataController.getHoldingBuf(REG_HOLDING_IS_SLAVE_COLOR_CHANGE_ADDR - REG_HOLDING_START));


        if (DataController.getHoldingBuf(REG_HOLDING_IS_MASTER_DATA_NEW_ADDR - REG_HOLDING_START) == 1){
            DataController.setHoldingBuf(REG_HOLDING_IS_MASTER_DATA_NEW_ADDR - REG_HOLDING_START, 0);
            QByteArray Data;
            DataController.getMasterMsg(&Data);
            QString DataAsString = QString(Data);
            object.insert("s_msg_data", DataAsString);
        }

        QJsonDocument doc(object);
        QByteArray bytes = doc.toJson();
        response.write(bytes, true);
    }

}

#endif
