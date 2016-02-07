#include "networkmanager.h"
#include "../util/fileutils.h"
#include "../util/jsonparser.h"
#include "../util/m3u8parser.h"

NetworkManager::NetworkManager(ChannelManager *cman)
{
    this->cman = cman;

    channelOperation = new QNetworkAccessManager();
    streamOperation = new QNetworkAccessManager();
    allStreamsOperation = new QNetworkAccessManager();
    genericFileOperation = new QNetworkAccessManager();
    gamesOperation = new QNetworkAccessManager();
    gameStreamsOperation = new QNetworkAccessManager();
    searchOperation = new QNetworkAccessManager();
    featuredStreamsOperation = new QNetworkAccessManager();
    extractChannelStreamsOperation = new QNetworkAccessManager();
    m3u8StreamsOperation = new QNetworkAccessManager();

    connect(channelOperation, SIGNAL(finished(QNetworkReply*)),this, SLOT(channelReply(QNetworkReply*)));
    connect(streamOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(streamReply(QNetworkReply*)));
    connect(genericFileOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileReply(QNetworkReply*)));
    connect(allStreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(allStreamsReply(QNetworkReply*)));
    connect(gamesOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamesReply(QNetworkReply*)));
    connect(searchOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(searchChannelsReply(QNetworkReply*)));
    connect(gameStreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(gameStreamsReply(QNetworkReply*)));
    connect(featuredStreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(featuredStreamsReply(QNetworkReply*)));
    connect(extractChannelStreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(channelPlaybackStreamReply(QNetworkReply*)));
    connect(m3u8StreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(m3u8Reply(QNetworkReply*)));
}

NetworkManager::~NetworkManager()
{
    qDebug() << "Destroyer: NetworkManager";
    delete channelOperation;
    delete streamOperation;
    delete genericFileOperation;
    delete allStreamsOperation;
    delete gamesOperation;
    delete searchOperation;
    delete gameStreamsOperation;
    delete featuredStreamsOperation;
    delete extractChannelStreamsOperation;
    delete m3u8StreamsOperation;
}

void NetworkManager::getChannel(const QString &uriName)
{
    //qDebug() << "GET channel: " << uriName;
    QString url = QString(KRAKEN_API) + "/channels/" + uriName;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    //request.setAttribute(QNetworkRequest::CustomVerbAttribute,channel->getUriName());

    channelOperation->get(request);
}

void NetworkManager::getStream(const QString &uriName)
{
    //qDebug() << "GET stream: " << uriName;
    QString url = QString(KRAKEN_API) + "/streams/" + uriName;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CustomVerbAttribute,uriName);

    streamOperation->get(request);
}

void NetworkManager::getStreams(const QString &url)
{
    //qDebug() << "GET: " << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    allStreamsOperation->get(request);
}

void NetworkManager::getFile(const QString &url, const QString &filename)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CustomVerbAttribute,filename);

    genericFileOperation->get(request);
}

void NetworkManager::getGames(const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = KRAKEN_API;
    url += QString("/games/top?limit=%1").arg(limit)
            + QString("&offset=%1").arg(offset);
    request.setUrl(QUrl(url));

    gamesOperation->get(request);
}

void NetworkManager::searchChannels(const QString &query, const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + QString("/search/channels?q=%1").arg(query)
            + QString("&offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    request.setUrl(QUrl(url));

    searchOperation->get(request);
}

void NetworkManager::getFeaturedStreams()
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + "/streams/featured?limit=25&offset=0";
    request.setUrl(QUrl(url));

    //qDebug() << url;

    featuredStreamsOperation->get(request);
}

void NetworkManager::getStreamsForGame(const QString &game, const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = QString(KRAKEN_API)
            + QString("/streams?game=%1").arg(game)
            + QString("&offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    request.setUrl(QUrl(url));

    gameStreamsOperation->get(request);
}

void NetworkManager::getChannelPlaybackStream(const QString &channelName)
{
    QString url = QString(TWITCH_API)
            + QString("/channels/%1").arg(channelName)
            + QString("/access_token");
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    extractChannelStreamsOperation->get(request);
}

void NetworkManager::getChannelm3u8(const QString &url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    m3u8StreamsOperation->get(request);
}

void NetworkManager::channelReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    QList<Channel*> list;
    list.append(JsonParser::parseChannel(data));
    cman->updateFavourites(list);
}

void NetworkManager::streamReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        if (reply->error() == QNetworkReply::ContentNotFoundError){
            //model->channelNotFound(channel);
        }
        return;
    }

    QByteArray data = reply->readAll();
    //qDebug() << data;

    QList<Channel*> list;
    list.append(JsonParser::parseStream(data));
    cman->updateStreams(list);
}

void NetworkManager::allStreamsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();
    //qDebug() << data;

    cman->updateStreams(JsonParser::parseStreams(data));
}

void NetworkManager::fileReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();
    QString filename = reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString();
    util::writeBinaryFile(filename,data);
}

void NetworkManager::gamesReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();
    //qDebug() << data;

    cman->addGames(JsonParser::parseGames(data));
}

void NetworkManager::gameStreamsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    cman->addSearchResults(JsonParser::parseStreams(data));
}

void NetworkManager::featuredStreamsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    cman->addFeaturedResults(JsonParser::parseFeatured(data));
}

void NetworkManager::searchChannelsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    cman->addSearchResults(JsonParser::parseChannels(data));
}

void NetworkManager::channelPlaybackStreamReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    //qDebug() << data;

    getChannelm3u8(JsonParser::parseChannelStreamExtractionInfo(data));
}

void NetworkManager::m3u8Reply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    //qDebug() << data;

    cman->foundPlaybackStream(m3u8::getUrls(data));
}
