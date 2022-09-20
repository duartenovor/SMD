#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "inc/smd.h"

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/pages/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    CSMD smd;

     QQmlContext *ctx = engine.rootContext();
     ctx->setContextProperty("smd", &smd);

    engine.load(url);

    return app.exec();
}
