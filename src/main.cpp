
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QScreen>
#include <QQmlContext>
#include <QFont>
#include <QFontDatabase>
#include "model/channelmanager.h"
#include "libmpv/mpvrenderer.h"

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    std::setlocale(LC_NUMERIC, "C");

    ChannelManager *cman = new ChannelManager();
    cman->checkResources();
    cman->load();

    QQmlApplicationEngine engine;

    qDebug() << "DPI ratio: " << QGuiApplication::primaryScreen()->physicalDotsPerInch() * QGuiApplication::primaryScreen()->devicePixelRatio();

    qDebug() << "Setting context variables...";
    engine.rootContext()->setContextProperty("g_cman", cman);
    engine.rootContext()->setContextProperty("g_ppi", QVariant::fromValue(QGuiApplication::primaryScreen()->physicalDotsPerInch() * QGuiApplication::primaryScreen()->devicePixelRatio()));
    engine.rootContext()->setContextProperty("g_favourites", cman->getFavouritesProxy());
    engine.rootContext()->setContextProperty("g_results", cman->getResultsModel());
    engine.rootContext()->setContextProperty("g_featured", cman->getFeaturedProxy());
    engine.rootContext()->setContextProperty("g_games", cman->getGamesModel());

    qmlRegisterType<MpvObject>("mpv", 1, 0, "MpvObject");

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    qDebug() << "Starting window...";
    app.exec();

    qDebug() << "Closing application...";
    delete cman;
}
