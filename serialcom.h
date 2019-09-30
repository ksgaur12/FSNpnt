#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <QObject>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QUdpSocket>
#include <QNetworkProxy>

#include "mavlink/mavlink_types.h"
//#include "filemanager.h"


class Serialcom : public QObject
{
    Q_OBJECT
public:
    explicit Serialcom(QObject *parent = nullptr);
    void writeData(const char *data, int size);
    void writeData(QByteArray &data);

    static QSerialPort *serial;

    mavlink_channel_t mav_ch = MAVLINK_COMM_0;

    uint8_t systemId;
    uint8_t componentId;
    uint8_t systemStatus;

    enum conn_type{
        SERIAL,
        UDP,
        TCP,
        IOT,
        NONE
    };

    conn_type conn;

private:
    void _handle_msg(mavlink_message_t *msg);
    void _send_heartbeat();

    bool _conn_stat;
    QThread _timer_thread;
    QTime _timer;
    QTimer _conn_timer;

    static QUdpSocket *udp_sock;
    QHostAddress _sender;
    quint16 _senderPort;

signals:
    void send_conn_status(bool status);
    void send_data_status(bool data_status);
    void send_serial_port(QString port);

public slots:
    void connect_telem(QString port, int baud, QString com_link_status);
    void readData();
    void search_port();

private slots:
    void _check_for_data();
    void _sendBytes(QByteArray data);
};

#endif // SERIALCOM_H
