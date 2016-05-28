#include "networkmanager.h"
#include "../util/fileutils.h"
#include "../util/jsonparser.h"
#include "../util/m3u8parser.h"
#include <QNetworkConfiguration>
#include <QEventLoop>

NetworkManager::NetworkManager()
{
    operation = new QNetworkAccessManager();

    //Select interface
    selectNetworkInterface();

    //SSL errors handle (down the drain)
    connect(operation, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));

    //Handshake
    operation->connectToHost(TWITCH_API);
}

NetworkManager::~NetworkManager()
{
    qDebug() << "Destroyer: NetworkManager";
    operation->deleteLater();
}

void NetworkManager::selectNetworkInterface()
{
    //Chooses a working network interface from interfaces list, if default configuration doesn't work

    QNetworkConfigurationManager conf;
    connectionOK = false;
    QString identifier;

    QEventLoop loop;
    connect(this, SIGNAL(finishedConnectionTest()), &loop, SLOT(quit()));

    //Test default configuration
    operation->setConfiguration(conf.defaultConfiguration());

    testConnection();
    loop.exec();

    if (connectionOK == true) {
        qDebug() << "Selected default network configuration";
        return;
    }

    else {
        qDebug() << "Failure on default configuration, attempt to choose another interaface..";

        foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces())
        {
            if (!interface.isValid())
                continue;

//            qDebug() << "Identifier: " << interface.name();
//            qDebug() << "HW addr: " << interface.hardwareAddress();

            bool isUp = interface.flags().testFlag(QNetworkInterface::IsUp);
            bool isLoopback = interface.flags().testFlag(QNetworkInterface::IsLoopBack);
            bool isActive = interface.flags().testFlag(QNetworkInterface::IsRunning);
            bool isPtP = interface.flags().testFlag(QNetworkInterface::IsPointToPoint);

//            qDebug() << "Properties: ";
//            qDebug() << (isUp ? "Is up" : "Is down");

//            if (isLoopback)
//                qDebug() << "Loopback";

//            qDebug() << (isActive ? "Active" : "Inactive");

//            if (isPtP)
//                qDebug() << "Is Point-to-Point";

//            qDebug() << "";

            if (isUp && isActive && !isLoopback) {
                identifier = interface.name();
                qDebug() << "Testing connection for interface " << identifier;
                operation->setConfiguration(conf.configurationFromIdentifier(identifier));

                testConnection();
                loop.exec();

                if (connectionOK == true) {
                    qDebug() << "Success!";
                    return;
                }

                else
                    qDebug() << "Failure, trying another interface...";
            }

        }
    }
}

void NetworkManager::testConnection()
{
    QNetworkRequest request;
    request.setUrl(QUrl(TWITCH_API_BASE));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(testConnectionReply()));
}

void NetworkManager::testConnectionReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

//    if (reply->error() == QNetworkReply::NoError)
//        qDebug() << "Got response: " << reply->readAll();

    connectionOK = (reply->error() == QNetworkReply::NoError);

    emit finishedConnectionTest();
}

void NetworkManager::getStreams(const QString &url)
{
    //qDebug() << "GET: " << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(allStreamsReply()));
}

void NetworkManager::getGames(const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = KRAKEN_API;
    url += QString("/games/top?limit=%1").arg(limit)
            + QString("&offset=%1").arg(offset);
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(gamesReply()));
}

void NetworkManager::searchChannels(const QString &query, const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + QString("/search/channels?q=%1").arg(query)
            + QString("&offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(searchChannelsReply()));
}

void NetworkManager::searchGames(const QString &query)
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + QString("/search/games?q=%1").arg(query)
            + "&type=suggest";

    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(searchGamesReply()));
}

void NetworkManager::searchGamesReply()
{

    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    emit searchGamesOperationFinished(JsonParser::parseGames(data));

    reply->deleteLater();
}

void NetworkManager::getFeaturedStreams()
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + "/streams/featured?limit=25&offset=0";
    request.setUrl(QUrl(url));

    //qDebug() << url;

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(featuredStreamsReply()));
}

void NetworkManager::getStreamsForGame(const QString &game, const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + QString("/streams?game=%1").arg(game)
            + QString("&offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(gameStreamsReply()));
}

void NetworkManager::getChannelPlaybackStream(const QString &channelName)
{
    QString url = QString(TWITCH_API)
            + QString("/channels/%1").arg(channelName)
            + QString("/access_token");
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    request.setAttribute(QNetworkRequest::User, LIVE);

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(streamExtractReply()));
}

void NetworkManager::getBroadcasts(const QString channelName, quint32 offset, quint32 limit)
{
    QString url = QString(KRAKEN_API)
            + QString("/channels/%1/videos").arg(channelName)
            + QString("?offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);

    if (ONLY_BROADCASTS)
        url += "&broadcasts=true";

    if (USE_HLS)
        url += "&hls=true";

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(broadcastsReply()));
}

void NetworkManager::getBroadcastPlaybackStream(const QString &vod)
{
    QString url = QString(TWITCH_API)
            + QString("/vods/%1").arg(vod)
            + QString("/access_token");
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    request.setAttribute(QNetworkRequest::User, VOD);

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(streamExtractReply()));
}

void NetworkManager::getUser(const QString &access_token)
{
    QString url = QString(KRAKEN_API) + "/user";
    QString auth = "OAuth " + access_token;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QString("Authorization").toUtf8(), auth.toUtf8());

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(userReply()));
}

void NetworkManager::getUserFavourites(const QString &user_name, quint32 offset, quint32 limit)
{
    QString url = QString(KRAKEN_API) + "/users/" + user_name + "/follows/channels"
            + QString("?offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(favouritesReply()));
}

void NetworkManager::editUserFavourite(const QString &access_token, const QString &user, const QString &channel, bool add)
{
    QString url = QString(KRAKEN_API) + "/users/" + user
            + "/follows/channels/" + channel;

    QString auth = "OAuth " + access_token;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QString("Authorization").toUtf8(), auth.toUtf8());

    QNetworkReply *reply = 0;

    if (add)
        reply = operation->put(request, QByteArray());
    else
        reply = operation->deleteResource(request);

    connect(reply, SIGNAL(finished()), this, SLOT(editUserFavouritesReply()));
}

QNetworkAccessManager *NetworkManager::getManager() const
{
    return operation;
}

void NetworkManager::getM3U8Data(const QString &url, M3U8TYPE type)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    request.setAttribute(QNetworkRequest::User, type);

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(m3u8Reply()));
}

void NetworkManager::handleSslErrors(QNetworkReply *reply, QList<QSslError> errors)
{
    reply->ignoreSslErrors(errors);
}

void NetworkManager::allStreamsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    emit allStreamsOperationFinished(JsonParser::parseStreams(data));

    reply->deleteLater();
}

void NetworkManager::gamesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();
    //qDebug() << data;

    emit gamesOperationFinished(JsonParser::parseGames(data));

    reply->deleteLater();
}

void NetworkManager::gameStreamsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    emit gameStreamsOperationFinished(JsonParser::parseStreams(data));

    reply->deleteLater();
}

void NetworkManager::featuredStreamsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    emit featuredStreamsOperationFinished(JsonParser::parseFeatured(data));

    reply->deleteLater();
}

void NetworkManager::searchChannelsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    emit searchChannelsOperationFinished(JsonParser::parseChannels(data));

    reply->deleteLater();
}

void NetworkManager::streamExtractReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();
    //qDebug() << data;

    M3U8TYPE type = (M3U8TYPE) reply->request().attribute(QNetworkRequest::User).toInt();

    QString url;

    switch (type) {
    case LIVE:
        url = JsonParser::parseChannelStreamExtractionInfo(data);
        break;

    case VOD:
        url = JsonParser::parseVodExtractionInfo(data);
        break;
    }

    getM3U8Data(url, type);

    reply->deleteLater();
}

void NetworkManager::m3u8Reply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    switch ((M3U8TYPE) reply->request().attribute(QNetworkRequest::User).toInt()) {
    case LIVE:
        emit m3u8OperationFinished(m3u8::getUrls(data));
        break;

    case VOD:
        emit m3u8OperationBFinished(m3u8::getUrls(data));
        break;
    }
    //qDebug() << data;

    reply->deleteLater();
}

void NetworkManager::broadcastsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    emit broadcastsOperationFinished(JsonParser::parseVods(data));

    reply->deleteLater();
}

void NetworkManager::favouritesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    emit favouritesReplyFinished(JsonParser::parseFavourites(data));

    reply->deleteLater();
}

void NetworkManager::editUserFavouritesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    //Nothing to do
    emit userEditFollowsOperationFinished();

    reply->deleteLater();
}

void NetworkManager::userReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    emit userNameOperationFinished(JsonParser::parseUserName(data));

    reply->deleteLater();
}
