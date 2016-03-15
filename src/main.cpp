
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQmlComponent>
#include <QQuickView>
#include <QScreen>
#include <QMainWindow>
#include <QQmlContext>
#include <QtSvg/QGraphicsSvgItem>
#include <QFontDatabase>
#include <QTimer>
#include "util/runguard.h"
#include "model/channelmanager.h"
#include "network/networkmanager.h"
#include "power/power.h"
#include "player/mpvrenderer.h"
#include "systray.h"
#include "customapp.h"
#include "util/notificationmaker.h"
#include "model/vodmanager.h"
#include <QString>
#include <QProcessEnvironment>

int main(int argc, char *argv[])
{
    //Force using "auto" value for QT_DEVICE_PIXEL_RATIO env var
    qputenv("QT_DEVICE_PIXEL_RATIO",QByteArray("auto"));

    CustomApp app(argc, argv);

    //Single application solution
    RunGuard guard("wz0dPKqHv3vX0BBsUFZt");
    if ( !guard.tryToRun() ){
        guard.sendWakeup();
        return -1;
    }

    QIcon appIcon = QIcon(":/icon/orion.ico");
    QApplication::setFont(QFont("qrc:/fonts/DroidSans.ttf"));

    app.setWindowIcon(appIcon);

    SysTray *tray = new SysTray();
    tray->setIcon(appIcon);

    QObject::connect(tray, SIGNAL(closeEventTriggered()), &app, SLOT(quit()));

    NetworkManager *netman = new NetworkManager();

    //Create channels manager
    ChannelManager *cman = new ChannelManager(netman);
    cman->checkResources();
    cman->load();

    //Screensaver mngr
    Power *power = new Power(static_cast<QApplication *>(&app));

    //Create vods manager
    VodManager *vod = new VodManager(netman);

//-------------------------------------------------------------------------------------------------------------------//

    QQmlApplicationEngine engine;

    qreal dpiMultiplier = QGuiApplication::primaryScreen()->physicalDotsPerInch() / QGuiApplication::primaryScreen()->devicePixelRatio();

#ifdef Q_OS_WIN
    dpiMultiplier /= 96;

#elif defined(Q_OS_LINUX)
    dpiMultiplier /= 96;

#elif defined(Q_OS_MAC)
    dpiMultiplier /= 72;

#endif

    qDebug() << "Pixel ratio " << QGuiApplication::primaryScreen()->devicePixelRatio();
    qDebug() <<"DPI mult: "<< dpiMultiplier;

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("dpiMultiplier", dpiMultiplier);
    rootContext->setContextProperty("g_cman", cman);
    rootContext->setContextProperty("g_guard", &guard);
    rootContext->setContextProperty("g_powerman", power);
    rootContext->setContextProperty("g_favourites", cman->getFavouritesProxy());
    rootContext->setContextProperty("g_results", cman->getResultsModel());
    rootContext->setContextProperty("g_featured", cman->getFeaturedProxy());
    rootContext->setContextProperty("g_games", cman->getGamesModel());
    rootContext->setContextProperty("g_tray", tray);
    rootContext->setContextProperty("g_vodmgr", vod);
    rootContext->setContextProperty("vodsModel", vod->getModel());

    std::setlocale(LC_NUMERIC, "C");
    qmlRegisterType<MpvObject>("mpv", 1, 0, "MpvObject");

    engine.load(QUrl("qrc:/main.qml"));

    //Set up notifications
    NotificationMaker *notificator = new NotificationMaker(&engine);
    QObject::connect(cman, SIGNAL(pushNotification(QString,QString,QString)), notificator, SLOT(pushNotification(QString,QString,QString)));

    qDebug() << "Starting window...";
    tray->show();

    app.exec();

//-------------------------------------------------------------------------------------------------------------------//

    //Cleanup
    delete vod;
    delete tray;
    delete notificator;
    delete netman;
    delete cman;

    qDebug() << "Closing application...";
    return 0;
}
