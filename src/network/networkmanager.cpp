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

#include "networkmanager.h"
#include "../util/fileutils.h"
#include "../util/jsonparser.h"
#include "../util/m3u8parser.h"
#include <QNetworkConfiguration>
#include <QEventLoop>

NetworkManager::NetworkManager(QNetworkAccessManager *man)
{
    operation = man;

    initReplayChat();

    //Select interface
    connectionOK = false;
    testNetworkInterface();

    //SSL errors handle (down the drain)
    connect(operation, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));

    //Handshake
    operation->connectToHost(TWITCH_API);

    //Set up offline poller
    offlinePoller.setInterval(2000);
    connect(&offlinePoller, SIGNAL(timeout()), this, SLOT(testNetworkInterface()));

    lastVodChatRequest = nullptr;
}

NetworkManager::~NetworkManager()
{
    offlinePoller.stop();
    qDebug() << "Destroyer: NetworkManager";
    //operation->deleteLater();
    teardownReplayChat();
}

void NetworkManager::testNetworkInterface()
{
    //Chooses a working network interface from interfaces list, if default configuration doesn't work

    QNetworkConfigurationManager conf;
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

    if (!connectionOK) {
        operation->setConfiguration(conf.defaultConfiguration());
        offlinePoller.start();
    }
}

bool NetworkManager::networkAccess() {
    return connectionOK;
}

void NetworkManager::testConnection()
{
    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(TWITCH_API_BASE));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(testConnectionReply()));
}

void NetworkManager::testConnectionReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

//    if (reply->error() == QNetworkReply::NoError)
//        qDebug() << "Got response: " << reply->readAll();

    handleNetworkError(reply);

    emit finishedConnectionTest();
}

/**
 * @brief NetworkManager::getStream
 * Gets single stream status. Usable for polling a channel's stream
 */
void NetworkManager::getStream(const QString &channelName)
{
    QString url = KRAKEN_API + QString("/streams/%1").arg(channelName);
    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(streamReply()));
}

void NetworkManager::getStreams(const QString &url)
{
    //qDebug() << "GET: " << url;
    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(allStreamsReply()));
}

void NetworkManager::getGames(const quint32 &offset, const quint32 &limit)
{
    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    QString url = QString(KRAKEN_API)
            + QString("/search/games?q=%1").arg(query)
            + "&type=suggest";

    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(searchGamesReply()));
}

void NetworkManager::getFeaturedStreams()
{
    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));
    request.setRawHeader(QString("Authorization").toUtf8(), auth.toUtf8());

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(userReply()));
}

void NetworkManager::getUserFavourites(const QString &user_name, quint32 offset, quint32 limit)
{
    if (user_name.isEmpty()) {
        return;
    }

    QString url = QString(KRAKEN_API) + "/users/" + user_name + "/follows/channels"
            + QString("?offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit);
    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::User, (int) (offset + limit));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(favouritesReply()));
}

void NetworkManager::getEmoteSets(const QString &access_token, const QList<int> &emoteSetIDs) {
    QList<QString> emoteSetsIDsStr;
    for (auto id : emoteSetIDs) {
        emoteSetsIDsStr.append(QString::number(id));
    }

    QString url = QString(KRAKEN_API) + "/chat/emoticon_images"
        + QString("?emotesets=") + emoteSetsIDsStr.join(',');
    QString auth = "OAuth " + access_token;

    qDebug() << "Requesting" << url;

    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));
    request.setRawHeader(QString("Authorization").toUtf8(), auth.toUtf8());

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(emoteSetsReply()));
}

void NetworkManager::getVodStartTime(quint64 vodId) {
    QString url = QString(TWITCH_RECHAT_API) + QString("?start=0&video_id=v%1").arg(vodId);

    qDebug() << "Failing request to get offset";
    qDebug() << "Request" << url;

    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(vodStartReply()));

}

void NetworkManager::getVodChatPiece(quint64 vodId, quint64 offset) {
    QString url = QString(TWITCH_RECHAT_API) + QString("?start=%1&video_id=v%2").arg(offset).arg(vodId);

    qDebug() << "Requesting" << url;

    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply *reply = operation->get(request);

    lastVodChatRequest = reply;
    
    connect(reply, SIGNAL(finished()), this, SLOT(vodChatPieceReply()));
}

void NetworkManager::cancelLastVodChatRequest() {
    if (lastVodChatRequest != nullptr) {
        lastVodChatRequest->abort();
        lastVodChatRequest = nullptr;
    }
}

void NetworkManager::resetVodChat() {
    replayChatPartNum = 0;
    curChatReplayDedupeBatch->clear();
    prevChatReplayDedupeBatch->clear();
}

void NetworkManager::initReplayChat() {
    curChatReplayDedupeBatch = new QSet<QString>();
    prevChatReplayDedupeBatch = new QSet<QString>();
}

void NetworkManager::teardownReplayChat() {
    delete curChatReplayDedupeBatch;
    delete prevChatReplayDedupeBatch;
}

void NetworkManager::filterReplayChat(QList<ReplayChatMessage> & replayChat) {
    if (replayChatPartNum % REPLAY_CHAT_DEDUPE_SWAP_ITERATIONS == 0) {
        auto oldPrev = prevChatReplayDedupeBatch;
        prevChatReplayDedupeBatch = curChatReplayDedupeBatch;
        oldPrev->clear();
        curChatReplayDedupeBatch = oldPrev;
    }

    for (auto entry = replayChat.begin(); entry != replayChat.end(); ) {
        if (entry->deleted || curChatReplayDedupeBatch->contains(entry->id) || prevChatReplayDedupeBatch->contains(entry->id)) {
            qDebug() << "DUPE" << entry->from << ":" << entry->id << entry->message;
            entry = replayChat.erase(entry);
        }
        else {
            //qDebug() << "GOOD" << entry->from << ":" << entry->id << entry->message;
            curChatReplayDedupeBatch->insert(entry->id);
            entry++;
        }
    }

    replayChatPartNum++;
}

void NetworkManager::vodStartReply() {
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 400) {
        if (!handleNetworkError(reply)) {
            return;
        }
    }

    QByteArray data = reply->readAll();

    //qDebug() << data;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    double startTime = 0.0;

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        //error_text = d['errors'][0]['detail']
        QString chatReplayErrorMessage = json["errors"].toArray()[0].toObject()["detail"].toString();
        // "not between (\d +) and (\d+)

        const QString START_MARKER = "not between ";

        int startMarkerPos = chatReplayErrorMessage.indexOf(START_MARKER);
        if (startMarkerPos == -1) {
            qDebug() << "chat replay error message in unexpected format:" << chatReplayErrorMessage;
            return;
        }

        int timePos = startMarkerPos + START_MARKER.length();
        QString timeStr = chatReplayErrorMessage.mid(timePos);
        int timeEnd = timeStr.indexOf(' ');
        if (timeEnd != -1) {
            timeStr = timeStr.left(timeEnd);
        }
        qDebug() << "timeStr" << timeStr;
        startTime = timeStr.toDouble();
    }

    emit vodStartGetOperationFinished(startTime);

    reply->deleteLater();

}

void NetworkManager::vodChatPieceReply() {
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (lastVodChatRequest == reply) {
        lastVodChatRequest = nullptr;
    }

    if (!handleNetworkError(reply)) {
        return;
    }

    QByteArray data = reply->readAll();

    //qDebug() << data;

    QList<ReplayChatMessage> ret = JsonParser::parseVodChatPiece(data);

    filterReplayChat(ret);

    emit vodChatPieceGetOperationFinished(ret);

    reply->deleteLater();
}

const QString CHANNEL_BADGES_URL_PREFIX = QString(KRAKEN_API) + "/chat/";
const QString CHANNEL_BADGES_URL_SUFFIX = "/badges";

void NetworkManager::getChannelBadgeUrls(const QString &access_token, const QString &channel) {
    QString url = CHANNEL_BADGES_URL_PREFIX + channel + CHANNEL_BADGES_URL_SUFFIX;
    QString auth = "OAuth " + access_token;

    qDebug() << "Requesting" << url;

    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));
    request.setRawHeader(QString("Authorization").toUtf8(), auth.toUtf8());

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(channelBadgeUrlsReply()));
}

const QString CHANNEL_BADGES_BETA_URL_PREFIX = "https://badges.twitch.tv/v1/badges/channels/";
const QString CHANNEL_BADGES_BETA_URL_SUFFIX = "/display?language=en";
const QString GLOBAL_BADGES_BETA_URL = "https://badges.twitch.tv/v1/badges/global/display?language=en";

void NetworkManager::getChannelBadgeUrlsBeta(const int channelID) {
    QString url = CHANNEL_BADGES_BETA_URL_PREFIX + QString::number(channelID) + CHANNEL_BADGES_BETA_URL_SUFFIX;

    qDebug() << "Requesting" << url;

    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(channelBadgeUrlsBetaReply()));
}

void NetworkManager::getGlobalBadgesUrlsBeta() {
    QString url = GLOBAL_BADGES_BETA_URL;

    qDebug() << "Requesting" << url;

    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(globalBadgeUrlsBetaReply()));
}

void NetworkManager::editUserFavourite(const QString &access_token, const QString &user, const QString &channel, bool add)
{
    QString url = QString(KRAKEN_API) + "/users/" + user
            + "/follows/channels/" + channel;

    QString auth = "OAuth " + access_token;

    QNetworkRequest request;
    request.setRawHeader("Client-ID", getClientId().toUtf8());
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
    request.setRawHeader("Client-ID", getClientId().toUtf8());
    request.setUrl(QUrl(url));

    request.setAttribute(QNetworkRequest::User, type);

    QNetworkReply *reply = operation->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(m3u8Reply()));
}

bool NetworkManager::handleNetworkError(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){

        if (reply->error() >= 1 && reply->error() <= 199) {

            if (connectionOK == true) {
                connectionOK = false;
                emit networkAccessChanged(false);
            }

            if (!offlinePoller.isActive())
                offlinePoller.start();
        }

        qDebug() << reply->errorString();

        return false;
    }

    if (!connectionOK) {
        connectionOK = true;
        emit networkAccessChanged(true);
    }

    if (offlinePoller.isActive())
        offlinePoller.stop();

    return true;
}

void NetworkManager::handleSslErrors(QNetworkReply *reply, QList<QSslError> errors)
{
    foreach (QSslError e, errors) {
        qDebug() << "Ssl error: " << e.errorString();
    }

    //reply->ignoreSslErrors(errors);
}

void NetworkManager::streamReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }

    QByteArray data = reply->readAll();

    //qDebug() << data;

    Channel *channel = JsonParser::parseStream(data);

    emit streamGetOperationFinished(channel->getServiceName(), channel->isOnline());

    channel->deleteLater();

    reply->deleteLater();
}

void NetworkManager::allStreamsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    emit allStreamsOperationFinished(JsonParser::parseStreams(data));

    reply->deleteLater();
}

void NetworkManager::searchGamesReply()
{

    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    emit searchGamesOperationFinished(JsonParser::parseGames(data));

    reply->deleteLater();
}

void NetworkManager::gamesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    emit gamesOperationFinished(JsonParser::parseGames(data));

    reply->deleteLater();
}

void NetworkManager::gameStreamsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
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

    if (!handleNetworkError(reply)) {
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

    if (!handleNetworkError(reply)) {
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

    if (!handleNetworkError(reply)) {
        emit error("token_error");
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

    if (!handleNetworkError(reply)) {

        emit error("playlist_error");

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

    if (!handleNetworkError(reply)) {
        return;
    }

    QByteArray data = reply->readAll();

    emit broadcastsOperationFinished(JsonParser::parseVods(data));

    reply->deleteLater();
}

void NetworkManager::favouritesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }

    QByteArray data = reply->readAll();

    int offset = reply->request().attribute(QNetworkRequest::User).toInt();

    emit favouritesReplyFinished(JsonParser::parseFavourites(data), offset);

    reply->deleteLater();
}

void NetworkManager::editUserFavouritesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }

    //Nothing to do
    emit userEditFollowsOperationFinished();

    reply->deleteLater();
}

void NetworkManager::userReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    emit userNameOperationFinished(JsonParser::parseUserName(data));

    reply->deleteLater();
}

void NetworkManager::emoteSetsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();
    
    emit getEmoteSetsOperationFinished(JsonParser::parseEmoteSets(data));

    reply->deleteLater();
}

void NetworkManager::channelBadgeUrlsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    QString urlString = reply->url().toString();

    qDebug() << "url was" << urlString;

    if (urlString.startsWith(CHANNEL_BADGES_URL_PREFIX) && urlString.endsWith(CHANNEL_BADGES_URL_SUFFIX)) {
        QString channel = urlString.mid(CHANNEL_BADGES_URL_PREFIX.length(), urlString.length() - CHANNEL_BADGES_URL_PREFIX.length() - CHANNEL_BADGES_URL_SUFFIX.length());
        qDebug() << "badges for channel" << channel << "loaded";
        auto badges = JsonParser::parseChannelBadgeUrls(data);

        emit getChannelBadgeUrlsOperationFinished(channel, badges);
    }
    else {
        qDebug() << "can't determine channel from badges request url";
    }


    reply->deleteLater();
}

void NetworkManager::channelBadgeUrlsBetaReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    QString urlString = reply->url().toString();

    qDebug() << "url was" << urlString;

    if (urlString.startsWith(CHANNEL_BADGES_BETA_URL_PREFIX) && urlString.endsWith(CHANNEL_BADGES_BETA_URL_SUFFIX)) {
        QString channelIDStr = urlString.mid(CHANNEL_BADGES_BETA_URL_PREFIX.length(), urlString.length() - CHANNEL_BADGES_BETA_URL_PREFIX.length() - CHANNEL_BADGES_BETA_URL_SUFFIX.length());
        int channelID = channelIDStr.toInt();
        qDebug() << "beta badges for channel" << channelID << "loaded";
        auto badges = JsonParser::parseBadgeUrlsBetaFormat(data);

        emit getChannelBadgeBetaUrlsOperationFinished(channelID, badges);
    }
    else {
        qDebug() << "can't determine channel from badges request url";
    }

    reply->deleteLater();
}

void NetworkManager::globalBadgeUrlsBetaReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (!handleNetworkError(reply)) {
        return;
    }
    QByteArray data = reply->readAll();

    QString urlString = reply->url().toString();

    qDebug() << "url was" << urlString;

    qDebug() << "global beta badges loaded";
    auto badges = JsonParser::parseBadgeUrlsBetaFormat(data);

    emit getGlobalBadgeBetaUrlsOperationFinished(badges);

    reply->deleteLater();
}

