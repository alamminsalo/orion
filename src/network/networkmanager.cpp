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


    connect(channelOperation, SIGNAL(finished(QNetworkReply*)),this, SLOT(channelReply(QNetworkReply*)));
    connect(streamOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(streamReply(QNetworkReply*)));
    connect(logoOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(logoReply(QNetworkReply*)));
    connect(genericFileOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileReply(QNetworkReply*)));
    connect(allStreamsOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(allStreamsReply(QNetworkReply*)));
    connect(gamesOperation, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamesReply(QNetworkReply*)));
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

void NetworkManager::getChannelData(Channel* channel)
{
    qDebug() << "GET channel: " << channel->getUriName();
    QString url = TWITCH_URI;
    url += "/channels/";
    url += channel->getUriName();
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CustomVerbAttribute,channel->getUriName());

    channelOperation->get(request);
}

void NetworkManager::getStream(Channel *channel)
{
    qDebug() << "GET stream: " << channel->getUriName();
    QString url = TWITCH_URI;
    url += "/streams/";
    url += channel->getUriName();
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CustomVerbAttribute,channel->getUriName());

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

void NetworkManager::getGames()
{
    QNetworkRequest request;
    QString url = TWITCH_URI;
    url += "/games/top?limit=50";
    request.setUrl(QUrl(url));

    gamesOperation->get(request);
}

void NetworkManager::channelReply(QNetworkReply* reply)
{
    Channel* channel = cman->find(reply->request().attribute(QNetworkRequest::CustomVerbAttribute).toString());
    if (channel){
        if (reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            if (reply->error() == QNetworkReply::ContentNotFoundError)
                cman->channelNotFound(channel);
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
            QJsonObject object = doc.object();
            cman->parseChannel(object,channel);
        }
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
            QJsonObject object = doc.object();
            if (!object["stream"].isNull())
                cman->parseStream(object["stream"].toObject(),channel);
            else
                getChannelData(channel);
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
        cman->parseStreams(doc.object());
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
        cman->parseGames(doc.object());
    }
}
