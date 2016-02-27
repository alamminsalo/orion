
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
#include "power/power.h"
#include "player/mpvrenderer.h"
#include "systray.h"
#include "customapp.h"
#include "util/notificationmaker.h"
#include <QProcessEnvironment>

int main(int argc, char *argv[])
{
    CustomApp app(argc, argv);

    //Single application solution
    RunGuard guard("wz0dPKqHv3vX0BBsUFZt");
    if ( !guard.tryToRun() ){
        guard.sendWakeup();
        return -1;
    }

    QIcon appIcon = QIcon(":/icon/orion.ico");

    app.setWindowIcon(appIcon);

    SysTray *tray = new SysTray();
    tray->setIcon(appIcon);

    QObject::connect(tray, SIGNAL(closeEventTriggered()), &app, SLOT(quit()));

    ChannelManager *cman = new ChannelManager();
    cman->checkResources();
    cman->load();

    Power *power = new Power();

    QQmlApplicationEngine engine;

#ifdef Q_OS_WIN
    qreal dpiMultiplier = QGuiApplication::primaryScreen()->logicalDotsPerInch() / 96;

#elif defined(Q_OS_LINUX)
    qreal dpiMultiplier = QGuiApplication::primaryScreen()->logicalDotsPerInch() / 72;

#endif

    qDebug() <<"DPI mult: "<< dpiMultiplier;

    qDebug() << "Setting context variables...";
    engine.rootContext()->setContextProperty("dpiMultiplier", dpiMultiplier);
    engine.rootContext()->setContextProperty("g_cman", cman);
    engine.rootContext()->setContextProperty("g_guard", &guard);
    engine.rootContext()->setContextProperty("g_powerman", power);
    engine.rootContext()->setContextProperty("g_favourites", cman->getFavouritesProxy());
    engine.rootContext()->setContextProperty("g_results", cman->getResultsModel());
    engine.rootContext()->setContextProperty("g_featured", cman->getFeaturedProxy());
    engine.rootContext()->setContextProperty("g_games", cman->getGamesModel());
    engine.rootContext()->setContextProperty("g_tray", tray);

    std::setlocale(LC_NUMERIC, "C");
    qmlRegisterType<MpvObject>("mpv", 1, 0, "MpvObject");

    engine.load(QUrl("qrc:/main.qml"));

    //Set up notifications
    NotificationMaker *notificator = new NotificationMaker(&engine);
    QObject::connect(cman, SIGNAL(pushNotification(QString,QString,QString)), notificator, SLOT(pushNotification(QString,QString,QString)));

    qDebug() << "Starting window...";

    power->setWid(app.allWindows().at(0)->winId());
    qDebug() << "WindowId: " << app.allWindows().at(0)->winId();

    tray->show();

    app.exec();

    delete tray;

    delete notificator;

    qDebug() << "Closing application...";
    delete cman;

    return 0;
}
