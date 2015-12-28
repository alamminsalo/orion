
#ifdef _QML

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include "model/channelmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ChannelManager *cman = new ChannelManager();
    //cman->checkResources();
    cman->load();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("g_cman", cman);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    app.exec();
    cman->deleteLater();
}

#else

#include "ui/mainwindow.h"
#include <QApplication>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow main;
    main.show();

    return a.exec();
}

#endif

