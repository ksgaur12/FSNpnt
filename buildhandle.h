#ifndef BUILDHANDLE_H
#define BUILDHANDLE_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>


class BuildHandle : public QObject
{
    Q_OBJECT
public:
    explicit BuildHandle(QObject *parent = nullptr);
private:
    QString _CN; //comman name
    QString _O;  //name of organization
    QString _OU; //name of department
    QString _L;  //name of city
    QString _S;  //name of state
    QString _C;  //name of country
    QString _MAIL;//email id

    QString _pub_key;//dgca pub key location
    QString _pkey;   //firmware build key location

signals:
    void json_build_success();

public slots:
    void send_info(QString CN, QString O, QString OU, QString L, QString S, QString C, QString mail, QString dgca_key, QString pkey);
    void create_json(QString folder);
};

#endif // BUILDHANDLE_H
