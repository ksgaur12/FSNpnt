#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QtWidgets>

#include "buildhandle.h"
#include "certhandle.h"
#include "filemanager.h"
#include "filemodel.h"
#include "serialcom.h"
#include "firmwareupgradecontroller.h"

BuildHandle build_manager;
CertHandle cert_manager;
FileManager file_manager;
filemodel fc_dir_model;
Serialcom comm_link;

Q_DECLARE_METATYPE(QGCSerialPortInfo)

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    qRegisterMetaType<QGCSerialPortInfo>();

    app.setOrganizationName("Firmware Server");
    app.setOrganizationDomain("");
    app.setApplicationName("Firmware Server NPNT");

    QQmlApplicationEngine engine;

    qmlRegisterType<FirmwareUpgradeController>      ("firmwareController", 1, 0, "FirmwareUpgradeController");

    engine.rootContext()->setContextProperty("build_manager", &build_manager);
    engine.rootContext()->setContextProperty("cert_manager", &cert_manager);
    engine.rootContext()->setContextProperty("file_manager", &file_manager);
    engine.rootContext()->setContextProperty("fc_dir_model", &fc_dir_model);
    engine.rootContext()->setContextProperty("Serialcom", &comm_link);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
