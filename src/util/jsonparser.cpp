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

#include "jsonparser.h"
#include <QUrl>

QList<Channel*> JsonParser::parseStreams(const QByteArray &data)
{
    QList<Channel*> channels;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        //Online streams
        QJsonArray arr = json["streams"].toArray();
        foreach (const QJsonValue &item, arr){
            channels.append(JsonParser::parseStreamJson(item.toObject()));
        }

        //Offline streams
        QStringList cnames;
        if (!json["_links"].isNull() && !json["_links"].toObject()["self"].isNull()){
            QString query = QUrl::fromPercentEncoding(QUrl(json["_links"].toObject()["self"].toString()).query().toUtf8());
            if (!query.isEmpty()){
                query.remove(0, query.indexOf("channel=") + 8);
                query.truncate(query.indexOf('&'));
                cnames = query.split(",");
            }
        }
        if (channels.count() < cnames.count()){
            foreach (const Channel* channel, channels){
                cnames.removeOne(channel->getServiceName());
            }
            foreach(const QString & name, cnames){
                channels.append(new Channel(name));
            }
        }
    }

    return channels;
}

Channel *JsonParser::parseStream(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        return parseStreamJson(doc.object());
    }
    return new Channel();
}

Channel* JsonParser::parseStreamJson(const QJsonObject &json)
{
    Channel* channel = new Channel();

    QJsonObject jsonObj;

    if (!jsonObj["stream"].isNull()) {
        jsonObj = jsonObj["stream"].toObject();
    } else {
        jsonObj = json;
    }

    //Take name from _links
    QJsonObject links = jsonObj["_links"].toObject();
    if (!links["self"].isNull()){
        QString channelName = links["self"].toString();
        channelName.remove(0, channelName.lastIndexOf('/') + 1);
        channel->setServiceName(channelName);
    }

    if (!jsonObj["preview"].isNull()){

        QJsonObject preview = jsonObj["preview"].toObject();

        if (!preview["large"].isNull()){
            channel->setPreviewurl(preview["large"].toString());
        }
    }

    if (!jsonObj["viewers"].isNull()){
        channel->setViewers(jsonObj["viewers"].toInt());
    }

    if (!jsonObj["game"].isNull()){
        channel->setGame(jsonObj["game"].toString());
    }

    if (!jsonObj["channel"].isNull()){

        Channel *c = parseChannelJson(jsonObj["channel"].toObject());
        channel->setServiceName(c->getServiceName());
        channel->setId(c->getId());
        channel->setName(c->getName());
        channel->setLogourl(c->getLogourl());
        channel->setInfo(c->getInfo());

        delete c;
    }

    channel->setOnline(true);

    return channel;
}

QList<Game*> JsonParser::parseGames(const QByteArray &data)
{
    QList<Game*> games;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        QString arg = (!json["top"].isNull() ? "top" : (!json["games"].isNull() ? "games" : ""));

        if (!arg.isEmpty()){
            QJsonArray arr = json[arg].toArray();
            foreach (const QJsonValue &item, arr){
                Game* game = parseGame(item.toObject());
                if (!game->getName().isEmpty()){
                    games.append(game);
                }
            }
        }
    }

    return games;
}


Game* JsonParser::parseGame(const QJsonObject &json)
{
    Game* game = new Game();

    //From top games
    if (json.contains("game") && !json["game"].isNull()){
        const QJsonObject gameObj = json["game"].toObject();

        if (!gameObj["_id"].isNull())
            game->setId(gameObj["_id"].toInt());

        if (!json["viewers"].isNull())
            game->setViewers(json["viewers"].toInt());

        if (!gameObj["name"].isNull())
            game->setName(gameObj["name"].toString());

        if (!gameObj["box"].isNull() && !gameObj["box"].toObject()["medium"].isNull())
            game->setLogo(gameObj["box"].toObject()["medium"].toString());

        if (!gameObj["logo"].isNull() && !gameObj["logo"].toObject()["medium"].isNull())
            game->setPreview(gameObj["logo"].toObject()["medium"].toString());
    }
    //From games search
    else {
        if (!json["_id"].isNull())
            game->setId(json["_id"].toInt());

        if (!json["name"].isNull())
            game->setName(json["name"].toString());

        if (!json["viewers"].isNull())
            game->setViewers(json["viewers"].toInt());

        if (!json["box"].isNull() && !json["box"].toObject()["medium"].isNull())
            game->setLogo(json["box"].toObject()["medium"].toString());

        if (!json["logo"].isNull() && !json["logo"].toObject()["medium"].isNull())
            game->setPreview(json["logo"].toObject()["medium"].toString());
    }

    return game;
}

Channel* JsonParser::parseChannel(const QByteArray &data){
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        return parseChannelJson(doc.object());
    }
    return new Channel();
}

Channel* JsonParser::parseChannelJson(const QJsonObject &json)
{
    Channel* channel = new Channel();

    if (!json["name"].isNull()){

        channel->setServiceName(json["name"].toString());

       // qDebug() << "Parsing channel data for " <<  channel.getUriName();

        if (!json["name"].isNull()){
            channel->setServiceName(json["name"].toString());
        }

        if (!json["display_name"].isNull()){
            channel->setName(json["display_name"].toString());
        }

        if (!json["status"].isNull()){
            channel->setInfo(json["status"].toString());
        }

        if (!json["logo"].isNull()){
            channel->setLogourl(json["logo"].toString());
        }

        if (!json["_id"].isNull()){
            channel->setId(json["_id"].toInt());
        }
    }

    return channel;
}

Vod *JsonParser::parseVod(const QJsonObject &json)
{
    Vod *vod = new Vod();

    if (!json["_id"].isNull())
        vod->setId(json["_id"].toString());

    if (!json["preview"].isNull())
        vod->setPreview(json["preview"].toString());

    if (!json["title"].isNull())
        vod->setTitle(json["title"].toString());

    if (!json["length"].isNull())
        vod->setDuration(json["length"].toInt());

    if (!json["game"].isNull())
        vod->setGame(json["game"].toString());

    if (!json["views"].isNull())
        vod->setViews(json["views"].toInt());

    if (!json["created_at"].isNull())
        vod->setCreatedAt(json["created_at"].toString());

    return vod;
}

QList<Channel*> JsonParser::parseChannels(const QByteArray &data)
{
    QList<Channel*> channels;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        QJsonArray arr = json["channels"].toArray();
        foreach (const QJsonValue &item, arr){
            channels.append(JsonParser::parseChannelJson(item.toObject()));
        }
    }

    return channels;
}

QList<Channel *> JsonParser::parseFavourites(const QByteArray &data)
{
    QList<Channel*> channels;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        QJsonArray arr = json["follows"].toArray();
        foreach (const QJsonValue &item, arr){
            channels.append(JsonParser::parseChannelJson(item.toObject()["channel"].toObject()));
        }
    }

    return channels;
}

QList<Channel *> JsonParser::parseFeatured(const QByteArray &data)
{
    QList<Channel*> channels;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        if (!json["featured"].isNull()){
            foreach (const QJsonValue &item, json["featured"].toArray()){
                channels.append(JsonParser::parseStreamJson(item.toObject()["stream"].toObject()));
            }
        }
    }

    return channels;
}

QList<Vod *> JsonParser::parseVods(const QByteArray &data)
{
    QList<Vod *> vods;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        if (!json["videos"].isNull()){
            foreach (const QJsonValue &item, json["videos"].toArray()){
                vods.append(JsonParser::parseVod(item.toObject()));
            }
        }
    }

    return vods;
}

QString JsonParser::parseChannelStreamExtractionInfo(const QByteArray &data)
{
    QString url;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        QString tokenData = json["token"].toString();

        //Strip escape markings and spaces
        //tokenData = tokenData.trimmed().remove("\\");

        QString channel;

        QJsonDocument tokenDoc = QJsonDocument::fromJson(tokenData.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            QJsonObject tokenJson = tokenDoc.object();
            channel = tokenJson["channel"].toString();
        }

        QString sig = json["sig"].toString();

        url = QString("http://usher.twitch.tv/api/channel/hls/%1").arg(channel + QString(".m3u8"))
                + QString("?player=twitchweb")
                + QString("&token=%1").arg(tokenData)
                + QString("&sig=%1").arg(sig)
                + QString("&allow_source=true&$allow_audio_only=true");
    }

    return url;
}

QString JsonParser::parseVodExtractionInfo(const QByteArray &data)
{
    QString url;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        QString tokenData = json["token"].toString();
        QString sig = json["sig"].toString();

        //Strip escape markings and spaces
        tokenData = tokenData.trimmed().remove("\\");

        QString vod;

        QJsonDocument tokenDoc = QJsonDocument::fromJson(tokenData.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            QJsonObject tokenJson = tokenDoc.object();
            vod = QString::number(tokenJson["vod_id"].toInt());
        }

        url = QString("http://usher.twitch.tv/vod/%1").arg(vod)
                + QString("?nauth=%1").arg(tokenData)
                + QString("&nauthsig=%1").arg(sig)
                + QString("&p=%1").arg(qrand() * 999999)
                + "&type=any"
                  "&player=twitchweb"
                  "&allow_source=true"
                  "&allow_audio_only=true";
    }

    return url;
}

QString JsonParser::parseUserName(const QByteArray &data)
{
    QString displayName;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);

    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();
        if (!json["name"].isNull())
            displayName = json["name"].toString();
    }

    return displayName;
}

QMap<int, QMap<int, QString>> JsonParser::parseEmoteSets(const QByteArray &data) {
    QMap<int, QMap<int, QString>> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    //qDebug() << "parsing emote sets response" << data;

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();
        if (!json["emoticon_sets"].isNull()) {
            auto emoticon_sets = json["emoticon_sets"].toObject();
            for (auto emoticonSetEntry = emoticon_sets.begin(); emoticonSetEntry != emoticon_sets.end(); emoticonSetEntry++) {
                auto emoticonSetID = emoticonSetEntry.key();
                QMap<int, QString> curSetEmoticons;
                auto emoticons = emoticonSetEntry.value().toArray();
                for (auto emoticonEntry = emoticons.begin(); emoticonEntry != emoticons.end(); emoticonEntry++) {
                    auto emoticonObj = emoticonEntry->toObject();
                    auto id = emoticonObj["id"];
                    auto code = emoticonObj["code"];
                    if (id.isDouble() && code.isString()) {
                        curSetEmoticons.insert(id.toInt(), code.toString());
                    }
                }
                int setId = emoticonSetID.toInt();
                //qDebug() << "saving set id" << setId;
                out.insert(setId, curSetEmoticons);
            }
        }
    }

    return out;
}

QMap<QString, QMap<QString, QString>> JsonParser::parseChannelBadgeUrls(const QByteArray &data) {
    QMap<QString, QMap<QString, QString>> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();
        for (auto badgeEntry = json.begin(); badgeEntry != json.end(); badgeEntry++) {
            if (badgeEntry.value().isNull()) continue;
            QMap<QString, QString> urls;
            QJsonObject badgeEntryJson = badgeEntry.value().toObject();
            for (auto urlEntry = badgeEntryJson.begin(); urlEntry != badgeEntryJson.end(); urlEntry++) {
                urls.insert(urlEntry.key(), urlEntry.value().toString());
            }
            out.insert(badgeEntry.key(), urls);
        }
    }
            
    return out;
}

QMap<QString, QString> convertJsonStringMap(const QJsonObject & obj) {
    QMap<QString, QString> out;

    for (auto entry = obj.constBegin(); entry != obj.constEnd(); entry++) {
        if (entry.value().isString()) {
            out.insert(entry.key(), entry.value().toString());
        }
    }

    return out;
}

QMap<QString, QMap<QString, QMap<QString, QString>>> JsonParser::parseBadgeUrlsBetaFormat(const QByteArray &data) {
    QMap<QString, QMap<QString, QMap<QString, QString>>> out;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();
        if (!json["badge_sets"].isNull()) {
            auto badge_sets = json["badge_sets"].toObject();
            for (auto badge_set_entry = badge_sets.constBegin(); badge_set_entry != badge_sets.end(); badge_set_entry++) {
                QString badge_set_name = badge_set_entry.key();

                if (!badge_set_entry.value().isNull()) {
                    auto badge_set_json = badge_set_entry.value().toObject();
                    if (!badge_set_json["versions"].isNull()) {

                        QMap<QString, QMap<QString, QString>> loadedBadgeSet;

                        auto versions_json = badge_set_json["versions"].toObject();
                        for (auto version_entry = versions_json.constBegin(); version_entry != versions_json.constEnd(); version_entry++) {
                            QString version_str = version_entry.key();
                            
                            if (version_entry.value().isObject()) {
                                loadedBadgeSet.insert(version_str, convertJsonStringMap(version_entry.value().toObject()));
                            }
                        }

                        out.insert(badge_set_name, loadedBadgeSet);

                    }

                }
            }
        }
    }

    return out;
}

QMap<QString, QMap<QString, QString>> JsonParser::parseBitsUrlsFormat(const QByteArray &data)
{
    const QString BITS_THEME = "dark";
    const QString BITS_TYPE = "animated";
    const QString BITS_SIZE = "2";

    QMap<QString, QMap<QString, QString>> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        auto actions = json["actions"].toArray();
        for (const auto & actionEntry : actions) {
            
            QMap<QString, QString> actionMap;

            const QJsonObject & actionObj = actionEntry.toObject();
            QString actionPrefix = actionObj["prefix"].toString();

            const QJsonArray & tiers = actionObj["tiers"].toArray();
            for (const auto & tierEntry : tiers) {
                const QJsonObject & tierObj = tierEntry.toObject();

                int minBits = tierObj["min_bits"].toInt();

                const QString & url = tierObj["images"].toObject()[BITS_THEME].toObject()[BITS_TYPE].toObject()[BITS_SIZE].toString();

                qDebug() << "bits url for" << actionPrefix << "minBits" << minBits << "is" << url;
                actionMap.insert(QString::number(minBits), url);
            }

            if (actionMap.size() > 0) {
                out.insert(actionPrefix, actionMap);
            }
        }
    }

    return out;
}

int JsonParser::parseTotal(const QByteArray &data)
{
    int total = 0;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);

    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();
        if (!json["_total"].isNull())
            total = json["_total"].toInt();
    }

    return total;
}

ReplayChatMessage parseVodChatEntry(const QJsonValue &entry) {
    ReplayChatMessage out;
    
    const QJsonObject & entryObj = entry.toObject();

    const QJsonObject & attributes = entryObj["attributes"].toObject();

    out.from = attributes["from"].toString();
    out.deleted = attributes["deleted"].toBool();
    out.message = attributes["message"].toString();
    out.room = attributes["room"].toString();
    out.timestamp = attributes["timestamp"].toDouble();
    out.videoOffset = attributes["video-offset"].toDouble();
    out.command = attributes["command"].toString();

    auto tags = attributes["tags"].toObject();
    for (auto tagEntry = tags.constBegin(); tagEntry != tags.constEnd(); tagEntry++) {
        auto tagName = tagEntry.key();
        if (tagName == "emotes") {
            auto emotes = tagEntry.value().toObject();
            for (auto emoteEntry = emotes.constBegin(); emoteEntry != emotes.constEnd(); emoteEntry++) {
                int emoteId = emoteEntry.key().toInt();

                out.emoteList.append(emoteId);

                auto emotePairs = emoteEntry.value().toArray();
                for (auto emotePair : emotePairs) {
                    auto emotePairArray = emotePair.toArray();
                    if (emotePairArray.size() == 2) {
                        auto first = emotePairArray[0].toInt();
                        auto last = emotePairArray[1].toInt();
                        out.emotePositionsMap.insert(first, qMakePair(last, emoteId));
                    }
                }
            }

        }
        else if (tagName == "mod" || tagName == "subscriber" || tagName == "turbo") {
            out.tags.insert(tagName, tagEntry.value().toBool());
        }
        else {
            out.tags.insert(tagName, tagEntry.value().toString());
        }
    }

    out.id = entryObj["id"].toString();
        
    return out;
}

QList<ReplayChatMessage> JsonParser::parseVodChatPiece(const QByteArray &data)
{
    QList<ReplayChatMessage> out;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        if (!json["data"].isNull() && json["data"].isArray()) {
            const QJsonArray & chatEntries = json["data"].toArray();
            for (const auto & entry : chatEntries) {

                out.append(parseVodChatEntry(entry));
            }
        }
    }

    return out;
}

QMap<QString, QList<QString>> JsonParser::parseChatterList(const QByteArray &data)
{
    QMap<QString, QList<QString>> out;
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        QJsonObject chatters = json["chatters"].toObject();
        
        for (auto groupEntry = chatters.constBegin(); groupEntry != chatters.constEnd(); groupEntry++) {
            QList<QString> groupChatters;
            const QJsonArray & groupChattersJson = groupEntry.value().toArray();
            for (const auto & chatter : groupChattersJson) {
                groupChatters.append(chatter.toString());
            }
            out.insert(groupEntry.key(), groupChatters);
        }
    }

    return out;
    
}
