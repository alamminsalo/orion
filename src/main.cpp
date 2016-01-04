
#ifdef _QML
//
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QScreen>
#include <QQmlContext>
#include "libmpv/client.h"
#include "model/channelmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ChannelManager *cman = new ChannelManager();
    cman->checkResources();
    cman->load();

    QQmlApplicationEngine engine;

    qDebug() << "DPI ratio: " << QGuiApplication::primaryScreen()->physicalDotsPerInch() * QGuiApplication::primaryScreen()->devicePixelRatio();

    engine.rootContext()->setContextProperty("g_cman", cman);
    engine.rootContext()->setContextProperty("g_ppi", QVariant::fromValue(QGuiApplication::primaryScreen()->physicalDotsPerInch() * QGuiApplication::primaryScreen()->devicePixelRatio()));
    engine.rootContext()->setContextProperty("g_favourites", cman->getFavouritesProxy());
    engine.rootContext()->setContextProperty("g_results", cman->getResultsModel());
    engine.rootContext()->setContextProperty("g_featured", cman->getFeaturedProxy());
    engine.rootContext()->setContextProperty("g_games", cman->getGamesModel());

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));



//    qDebug() << "Initializing mpv...";

//    mpv_handle* mpv = mpv_create();

//    if (mpv_initialize(mpv) == MPV_ERROR_SUCCESS){
//        qDebug() << "Success!";
//    }

//    qDebug() << "libmpv version " << mpv_client_api_version();

//    int64_t wid = app.allWindows()[0]->winId();

//    qDebug() << "winId is " << wid;

//    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);

//    const char* asd = QString("play").toUtf8().constData();
//    mpv_command(mpv, &asd);

    app.exec();
    delete cman;
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

