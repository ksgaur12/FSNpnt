#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>

#include "filemanager.h"
#include "serialcom.h"
#include "filemodel.h"

Serialcom comm_link;
FileManager file_manager;
filemodel fc_dir_model;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("Serialcom", &comm_link);
    engine.rootContext()->setContextProperty("file_manager", &file_manager);
    engine.rootContext()->setContextProperty("fc_dir_model", &fc_dir_model);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
