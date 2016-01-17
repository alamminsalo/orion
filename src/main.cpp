
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QScreen>
#include <QQmlContext>
#include <QFont>
#include <QtSvg/QGraphicsSvgItem>
#include <QFontDatabase>
#include "model/channelmanager.h"
#include "power/power.h"
#include "player/mpvrenderer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icon/orion.ico"));

    ChannelManager *cman = new ChannelManager();
    cman->checkResources();
    cman->load();

    Power *power = new Power();

    QQmlApplicationEngine engine;

    qDebug() << "DPI ratio: " << QGuiApplication::primaryScreen()->physicalDotsPerInch() * QGuiApplication::primaryScreen()->devicePixelRatio();

    qDebug() << "Setting context variables...";
    engine.rootContext()->setContextProperty("g_cman", cman);
    engine.rootContext()->setContextProperty("g_powerman", power);
    engine.rootContext()->setContextProperty("g_ppi", QVariant::fromValue(QGuiApplication::primaryScreen()->physicalDotsPerInch() * QGuiApplication::primaryScreen()->devicePixelRatio()));
    engine.rootContext()->setContextProperty("g_favourites", cman->getFavouritesProxy());
    engine.rootContext()->setContextProperty("g_results", cman->getResultsModel());
    engine.rootContext()->setContextProperty("g_featured", cman->getFeaturedProxy());
    engine.rootContext()->setContextProperty("g_games", cman->getGamesModel());

    std::setlocale(LC_NUMERIC, "C");
    qmlRegisterType<MpvObject>("mpv", 1, 0, "MpvObject");

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    qDebug() << "Starting window...";

    power->setWid(app.allWindows().at(0)->winId());
    qDebug() << "WindowId: " << app.allWindows().at(0)->winId();

    app.exec();

    qDebug() << "Closing application...";
    delete cman;

    return 0;
}
