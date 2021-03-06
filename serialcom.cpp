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
QUdpSocket* Serialcom::udp_sock;

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
    if(conn == SERIAL)
        serial->write(data);
    else if(conn == UDP && _sender != QHostAddress::Null){
        udp_sock->writeDatagram(data, _sender, _senderPort);
    }
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
    if(conn == SERIAL)
        mav_data = serial->readAll();
    else if(conn== UDP){
        mav_data.resize(udp_sock->pendingDatagramSize());
        udp_sock->readDatagram(mav_data.data(), mav_data.size(), &_sender, &_senderPort);
    }

    for(int i=0; i< mav_data.size(); i++){
        if(mavlink_parse_char(mav_ch, mav_data.at(i), &msg, &com_status)){
            _handle_msg(&msg);
        }
    }
    _timer.restart();
    QTimer::singleShot(10000, this, SLOT(_check_for_data()));
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

        if(!_paramInit){
            const int maxStreams = 7;
            const uint8_t MAVStreams[] = { MAV_DATA_STREAM_RAW_SENSORS, MAV_DATA_STREAM_EXTENDED_STATUS, MAV_DATA_STREAM_RC_CHANNELS, MAV_DATA_STREAM_POSITION, MAV_DATA_STREAM_EXTRA1, MAV_DATA_STREAM_EXTRA2, MAV_DATA_STREAM_EXTRA3}; const uint16_t MAVRates[] = { 0x02, 0x05, 0x02, 0x05, 0x02, 0x02, 0x01};
            for (int i=0; i < maxStreams; i++) {
                for(int j=0; j<5; j++)
                    mavlink_msg_request_data_stream_send(MAVLINK_COMM_0, systemId,  componentId,  0, 5, 1);
            }
            _paramInit = true;

            mavlink_msg_param_request_list_send(MAVLINK_COMM_0, systemId, componentId);
        }

        //_send_heartbeat();
        break;
    case MAVLINK_MSG_ID_PERMISSION_ARTIFACT:
        mavlink_permission_artifact_t pa;
        mavlink_msg_permission_artifact_decode(msg, &pa);
        emit send_device_id(pa.busname);
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

void Serialcom::flight_reboot()
{
    mavlink_msg_command_long_send(MAVLINK_COMM_0, systemId, componentId, MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, 0, 1, 0, 0, 0, 0, 0, 0); //sends mavlink to set home as current
}

void Serialcom::connect_telem(QString port, int baud, QString com_link_status)
{
    if(com_link_status == "Disconnect"){
        if(conn == SERIAL){
            serial->close();
            delete serial;
        }
        else if(conn == UDP){
            udp_sock->close();
            delete udp_sock;
        }
        _conn_stat = false;
        emit send_conn_status(_conn_stat);
        return;
    }
    qDebug() << port;
    if(port == "UDP"){
        qDebug() << baud;
        switch(baud){
        case 8:
            baud = 14550;
            break;
        case 9:
            baud = 14551;
            break;
        case 10:
            baud = 14552;
            break;
        }
        udp_sock = new QUdpSocket();
        qDebug() << udp_sock->bind(QHostAddress::AnyIPv4, baud, QAbstractSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        udp_sock->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,     64 * 1024);
        udp_sock->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 128 * 1024);
        conn = UDP;
        connect(udp_sock, SIGNAL(readyRead()), this, SLOT(readData()));
        return;
    }

    serial = new QSerialPort();
    serial->setPortName(port);

    switch(baud){
    case 0:
        serial->setBaudRate(QSerialPort::Baud1200);
        break;
    case 1:
        serial->setBaudRate(QSerialPort::Baud2400);
        break;
    case 2:
        serial->setBaudRate(QSerialPort::Baud4800);
        break;
    case 3:
        serial->setBaudRate(QSerialPort::Baud9600);
        break;
    case 4:
        serial->setBaudRate(QSerialPort::Baud19200);
        break;
    case 5:
        serial->setBaudRate(QSerialPort::Baud38400);
        break;
    case 6:
        serial->setBaudRate(QSerialPort::Baud57600);
        break;
    case 7:
        serial->setBaudRate(QSerialPort::Baud115200);
        break;
    }

    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        conn = SERIAL;
        connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    }

}
