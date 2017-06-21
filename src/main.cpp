/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQmlComponent>
#include <QQuickView>
#include <QScreen>
//Nothing seems to happen when excluded. Are these needed?
//#include <QMainWindow>
//#include <QtSvg/QGraphicsSvgItem>
//#include <QString>
#include <QQmlContext>
#include <QFontDatabase>
#include <QResource>
#include "util/runguard.h"
#include "model/channelmanager.h"
#include "network/networkmanager.h"
#include "power/power.h"
#include "systray.h"
#include "customapp.h"
#include "notification/notificationmanager.h"
#include "model/vodmanager.h"
#include "model/ircchat.h"
#include "network/httpserver.h"
#include <QFont>
#include "model/viewersmodel.h"
#include "global.h"

#ifdef MPV_PLAYER
#include "player/mpvrenderer.h"
#endif

inline void noisyFailureMsgHandler(QtMsgType /*type*/, const QMessageLogContext &/*context*/, const QString &/*msg*/)
{

}

int main(int argc, char *argv[])
{
    CustomApp app(argc, argv);
    app.setApplicationVersion(APP_VERSION);

    const QIcon appIcon = QIcon(":/icon/orion.ico");
    app.setWindowIcon(appIcon);

    //Single application solution
    RunGuard guard("wz0dPKqHv3vX0BBsUFZt");
    if ( !guard.tryToRun() ){
        guard.sendWakeup();
        return -1;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("Desktop client for Twitch.tv");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption debugOption(QStringList() << "d" << "debug", "show debug output");
    parser.addOption(debugOption);
    parser.process(app);

    bool showDebugOutput = parser.isSet(debugOption);

    //Setup default font
    int id = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Regular.ttf");
    if (id == -1)
        qDebug() << "Can't open application font!";
    else
        app.setFont(QFont(QFontDatabase::applicationFontFamilies(id).first()));
    qDebug() << "App font:" << app.font().family();

#ifndef  QT_DEBUG
    if (!showDebugOutput) {
        qInstallMessageHandler(noisyFailureMsgHandler);
    }
#endif

    SysTray *tray = new SysTray(&app);
    tray->setIcon(appIcon);
    tray->show();

    QObject::connect(tray, &SysTray::closeEventTriggered, &app, &CustomApp::quit);

    //Prime network manager
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QQmlApplicationEngine engine;
    NetworkManager::initialize(engine.networkAccessManager());

    // detect hi dpi screens
    qDebug() << "Screens:";
    int screens = 0;
    qreal maxDevicePixelRatio = QGuiApplication::primaryScreen()->devicePixelRatio();
    for (const auto & screen : QGuiApplication::screens()) {
        qreal curPixelRatio = screen->devicePixelRatio();
        maxDevicePixelRatio = qMax(maxDevicePixelRatio, curPixelRatio);
        screens++;
        qDebug() << "  Screen #" << screens << screen->name() << ": devicePixelRatio" << curPixelRatio;
    }
    qDebug() << "maxDevicePixelRatio" << maxDevicePixelRatio;
    global::hiDpi = maxDevicePixelRatio > 1.0;
    qDebug() << "hiDpi" << global::hiDpi;

    //Screensaver mngr
    Power *power = new Power(static_cast<QApplication *>(&app));
    qreal dpiMultiplier = QGuiApplication::primaryScreen()->logicalDotsPerInch();

#ifdef Q_OS_WIN
    dpiMultiplier /= 96;

#elif defined(Q_OS_LINUX)
    dpiMultiplier /= 96;

#elif defined(Q_OS_MAC)
    dpiMultiplier /= 72;

#endif

    //Small adjustment to sizing overall
    dpiMultiplier *= .8;

    qDebug() << "Pixel ratio " << QGuiApplication::primaryScreen()->devicePixelRatio();
    qDebug() <<"DPI mult: "<< dpiMultiplier;

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("dpiMultiplier", dpiMultiplier);
    rootContext->setContextProperty("g_powerman", power);
    rootContext->setContextProperty("g_favourites", ChannelManager::getInstance()->getFavouritesProxy());
    rootContext->setContextProperty("g_results", ChannelManager::getInstance()->getResultsModel());
    rootContext->setContextProperty("g_games", ChannelManager::getInstance()->getGamesModel());
    rootContext->setContextProperty("vodsModel", VodManager::getInstance()->getModel());
    rootContext->setContextProperty("hiDPI", global::hiDpi);

#ifdef MPV_PLAYER
    rootContext->setContextProperty("player_backend", "mpv");
    qmlRegisterType<MpvObject>("mpv", 1, 0, "MpvObject");

#elif defined (QTAV_PLAYER)
    rootContext->setContextProperty("player_backend", "qtav");

#elif defined (MULTIMEDIA_PLAYER)
    rootContext->setContextProperty("player_backend", "multimedia");
#endif

    qmlRegisterSingletonType<ChannelManager>("app.orion", 1, 0, "ChannelManager", &ChannelManager::provider);
    qmlRegisterSingletonType<BadgeContainer>("app.orion", 1, 0, "Emotes", &BadgeContainer::provider);
    qmlRegisterSingletonType<ViewersModel>("app.orion", 1, 0, "Viewers", &ViewersModel::provider);
    qmlRegisterSingletonType<VodManager>("app.orion", 1, 0, "VodManager", &VodManager::provider);
    qmlRegisterSingletonType<SettingsManager>("app.orion", 1, 0, "Settings", &SettingsManager::provider);
    qmlRegisterSingletonType<HttpServer>("app.orion", 1, 0, "LoginService", &HttpServer::provider);
    qmlRegisterSingletonType<NetworkManager>("app.orion", 1, 0, "Network", &NetworkManager::provider);
    qmlRegisterType<IrcChat>("aldrog.twitchtube.ircchat", 1, 0, "IrcChat");

    //Setup obj parents for cleanup
    ChannelManager::getInstance()->setParent(&app);
    BadgeContainer::getInstance()->setParent(&app);
    ViewersModel::getInstance()->setParent(&app);
    VodManager::getInstance()->setParent(&app);
    SettingsManager::getInstance()->setParent(&app);
    HttpServer::getInstance()->setParent(&app);

    //Set up notifications
    NotificationManager *notificationManager = new NotificationManager(&engine, engine.networkAccessManager(), &app);
    QObject::connect(ChannelManager::getInstance(), &ChannelManager::pushNotification, notificationManager, &NotificationManager::pushNotification);

    // Load QML content
    engine.load(QUrl("qrc:/main.qml"));

    // Load app settings
    SettingsManager::getInstance()->load();

    // Get QML root window, add connections
    QQuickWindow *rootWin = (QQuickWindow *) engine.rootObjects().first();
    if (rootWin) {
        if (SettingsManager::getInstance()->minimizeOnStartup())
            rootWin->hide();

        //Connect to tray and runguard events
        QObject::connect(&guard, &RunGuard::anotherProcessTriggered, rootWin, &QQuickWindow::show);
        QObject::connect(tray, &SysTray::showTriggered, rootWin, [rootWin](){
            if (rootWin->isVisible())
                rootWin->hide();
            else
                rootWin->show();
        });
    }

    // Start
    return app.exec();
}
