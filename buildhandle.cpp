#include "buildhandle.h"
#include <QDebug>

BuildHandle::BuildHandle(QObject *parent) : QObject(parent)
{

}

void BuildHandle::send_info(QString CN, QString O, QString OU, QString L, QString S, QString C, QString mail, QString dgca_key, QString pkey)
{
    _CN = CN;
    _O  = O;
    _OU = OU;
    _L  = L;
    _S  = S;
    _C  = C;
    _MAIL = mail;
    _pub_key = dgca_key.remove(0, 7); //remove the prefix file
    _pkey   = pkey.remove(0, 7);
}

void BuildHandle::create_json(QString folder)
{
    folder.remove(0,7); //remove the prefix

    folder = folder + "/build_settings.json";

    QFile file(folder);
    file.open(QIODevice::WriteOnly);

    QJsonObject build_object;
    build_object.insert("CN", _CN);
    build_object.insert("O", _O);
    build_object.insert("OU", _OU);
    build_object.insert("L", _L);
    build_object.insert("S", _S);
    build_object.insert("C", _C);
    build_object.insert("MAIL", _MAIL);
    build_object.insert("pub_key", _pub_key);
    build_object.insert("secure_key", _pkey);

    QJsonDocument doc(build_object);
    qDebug() << doc.toJson();

    file.write(doc.toJson(), doc.toJson().size());
    file.close();

    emit json_build_success();
}

