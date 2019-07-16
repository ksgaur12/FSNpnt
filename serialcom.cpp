#include "serialcom.h"
#include "mav_bridge.h"
#include "filemanager.h"
#include <QDebug>

extern FileManager file_manager;
//extern filemodel fc_dir_model;

mavlink_system_t mavlink_system;
mavlink_message_t msg;
mavlink_status_t com_status;

QSerialPort* Serialcom::serial;

Serialcom::Serialcom(QObject *parent) : QObject(parent)
{
    _timer.start();
}
void Serialcom::_check_for_data(){
    if(_timer.elapsed() > 1000){
        emit send_data_status(false);
    }
}
void Serialcom::_sendBytes(QByteArray data){
    serial->write(data);
}

void Serialcom::writeData(const char *data, int size)
{
    _sendBytes(QByteArray(data, size));
}
void Serialcom::writeData(QByteArray &data)
{
    _sendBytes(data);
}

void Serialcom::readData()
{
    QByteArray mav_data;
    mav_data = serial->readAll();

    for(int i=0; i< mav_data.size(); i++){
        if(mavlink_parse_char(mav_ch, mav_data.at(i), &msg, &com_status)){
            _handle_msg(&msg);
        }
    }
    _timer.restart();
    QTimer::singleShot(1000, this, SLOT(_check_for_data()));
}

void Serialcom::_handle_msg(mavlink_message_t *msg){
    systemId = msg->sysid;
    componentId = msg->compid;

    switch(msg->msgid){
    case MAVLINK_MSG_ID_HEARTBEAT:
        mavlink_heartbeat_t heartbeat;
        mavlink_msg_heartbeat_decode(msg, &heartbeat);
        systemStatus = heartbeat.system_status;
        emit send_data_status(true);
        if(_conn_stat == false){
            _conn_stat = true;
            emit send_conn_status(_conn_stat);
        }
        _send_heartbeat();
        break;
    case MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL:
        mavlink_file_transfer_protocol_t file;
        mavlink_msg_file_transfer_protocol_decode(msg, &file);
        FileManager::Request* request = (FileManager::Request*)&file.payload[0];
        file_manager.receiveMessage(request);
        break;
    }
}

void Serialcom::_send_heartbeat(){
    mavlink_msg_heartbeat_send(mav_ch, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, MAV_MODE_FLAG_AUTO_ENABLED | MAV_MODE_FLAG_STABILIZE_ENABLED, 0, systemStatus);
}

void Serialcom::search_port()
{
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    for(int i = 0; i<serialPortInfos.length(); i++){
        emit send_serial_port(serialPortInfos.at(i).portName());
    }
}

void Serialcom::connect_telem(QString port, int baud, QString com_link_status)
{
    if(com_link_status == "Disconnect"){
        serial->close();
        _conn_stat = false;
        delete serial;
        emit send_conn_status(_conn_stat);
        return;
    }
    serial = new QSerialPort();
    serial->setPortName(port);

    switch(baud){
    case 1:
        serial->setBaudRate(QSerialPort::Baud1200);
        break;
    case 2:
        serial->setBaudRate(QSerialPort::Baud2400);
        break;
    case 3:
        serial->setBaudRate(QSerialPort::Baud4800);
        break;
    case 4:
        serial->setBaudRate(QSerialPort::Baud9600);
        break;
    case 5:
        serial->setBaudRate(QSerialPort::Baud19200);
        break;
    case 6:
        serial->setBaudRate(QSerialPort::Baud38400);
        break;
    case 7:
        serial->setBaudRate(QSerialPort::Baud57600);
        break;
    case 8:
        serial->setBaudRate(QSerialPort::Baud115200);
        break;
    }

    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    }

}
