#include "networkmanager.h"

NetworkManager::NetworkManager(ChannelManager *cman)
{
    this->cman = cman;

    channelOperation = new QNetworkAccessManager();
    streamOperation = new QNetworkAccessManager();
    logoOperation = new QNetworkAccessManager();
    allStreamsOperation = new QNetworkAccessManager();
    genericFileOperation = new QNetworkAccessManager();
    gamesOperation = new QNetworkAccessManager();
    searchOperation = new QNetworkAccessManager();

    connect(channelOperation, SIGNAL(finished(QNetworkReply*)),this, SLOT(channelReply(QNetworkReply*)));
    connect(streamOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(streamReply(QNetworkReply*)));
    connect(logoOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(logoReply(QNetworkReply*)));
    connect(genericFileOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileReply(QNetworkReply*)));
    connect(allStreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(allStreamsReply(QNetworkReply*)));
    connect(gamesOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamesReply(QNetworkReply*)));
    connect(searchOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(searchChannelsReply(QNetworkReply*)));
}

NetworkManager::~NetworkManager()
{
    qDebug() << "Destroyer: NetworkManager";
    delete channelOperation;
    delete streamOperation;
    delete logoOperation;
    delete allStreamsOperation;
    delete genericFileOperation;
}

void NetworkManager::getChannel(const QString &uriName)
{
    qDebug() << "GET channel: " << uriName;
    QString url = QString(TWITCH_URI) + "/channels/" + uriName;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    //request.setAttribute(QNetworkRequest::CustomVerbAttribute,channel->getUriName());

    channelOperation->get(request);
}

void NetworkManager::getStream(const QString &uriName)
{
    qDebug() << "GET stream: " << uriName;
    QString url = QString(TWITCH_URI) + "/streams/" + uriName;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    //request.setAttribute(QNetworkRequest::CustomVerbAttribute,channel->getUriName());

    streamOperation->get(request);
}

void NetworkManager::getAllStreams(const QString &url)
{
    qDebug() << "GET: " << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    allStreamsOperation->get(request);
}

void NetworkManager::getLogo(Channel *channel)
{
    QString url = channel->getLogourl();
    qDebug() << "GET LOGO for: " << channel->getUriName() << " from: " << url;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CustomVerbAttribute,channel->getUriName());

    logoOperation->get(request);
}

void NetworkManager::getFile(const QString &url, const QString &filename)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CustomVerbAttribute,filename);

    genericFileOperation->get(request);
}

void NetworkManager::getGames(quint32 limit, quint32 offset)
{
    QNetworkRequest request;
    QString url = TWITCH_URI;
    url += QString("/games/top?limit=%1").arg(limit)
            + QString("&offset=%2").arg(offset);
    request.setUrl(QUrl(url));

    gamesOperation->get(request);
}

void NetworkManager::searchChannels(const QString &query, const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    QString url = QString(TWITCH_URI)
            + QString("/search/channels?q=%1").arg(query)
            + QString("&offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    request.setUrl(QUrl(url));

    searchOperation->get(request);
}

void NetworkManager::channelReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
//        if (reply->error() == QNetworkReply::ContentNotFoundError)
//            cman->channelNotFound(channel);
        return;
    }

    QByteArray data = reply->readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError){
        qDebug() << "Error while parsing data: " << error.errorString();
        return;
    }
    if (doc.isObject()){
        cman->updateChannel(JsonParser::parseChannel(doc.object()));
    }
}

void NetworkManager::streamReply(QNetworkReply *reply)
{
    Channel* channel = cman->find(reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString());
    if (channel){
        if (reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            if (reply->error() == QNetworkReply::ContentNotFoundError){
                cman->channelNotFound(channel);
            }
            return;
        }

        QByteArray data = reply->readAll();
        //qDebug() << data;

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError){
            qDebug() << "Error while parsing data: " << error.errorString();
            return;
        }
        if (doc.isObject()){
            cman->updateStream(JsonParser::parseStream(doc.object()));
        }
    }
}

void NetworkManager::allStreamsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();
    //qDebug() << data;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        cman->updateStreams(JsonParser::parseStreams(doc.object()));
    }
}

void NetworkManager::logoReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();
    Channel* channel = cman->find(reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString());
    if (channel){
        if (util::writeBinaryFile(channel->getLogoPath(),data))
            channel->iconUpdated();
    }
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

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        cman->updateGames(JsonParser::parseGames(doc.object()));
    }
}

void NetworkManager::searchChannelsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    QByteArray data = reply->readAll();

    //qDebug() << data;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        cman->setResults(JsonParser::parseChannels(doc.object()));
    }
}
