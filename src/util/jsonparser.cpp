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
#include "../model/settingsmanager.h"

PagedResult<Channel*> JsonParser::parseStreams(const QByteArray &data)
{
    PagedResult<Channel*> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        //Online streams
        QJsonArray arr = json["streams"].toArray();
        foreach (const QJsonValue &item, arr){
            out.items.append(JsonParser::parseStreamJson(item.toObject(), true));
        }

        out.total = json["_total"].toInt();

        //Caller must use request context to determine offline streams
    }

    return out;
}

Channel *JsonParser::parseStream(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        return parseStreamJson(doc.object(), false);
    }
    return new Channel();
}

Channel* JsonParser::parseStreamJson(const QJsonObject &json, const bool expectChannel)
{
    Channel* channel = new Channel();

    QJsonObject jsonObj;

    if (!jsonObj["stream"].isNull()) {
        jsonObj = jsonObj["stream"].toObject();
    } else {
        jsonObj = json;
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
    else if (expectChannel) {
        qDebug() << "expected channel; stream will not have channel id to correlate";
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
            const QJsonValue & _id = json["_id"];
            channel->setId(_id.isString() ? _id.toString().toInt() : static_cast<quint32>(_id.toDouble()));
        }
    }

    return channel;
}

Vod *JsonParser::parseVod(const QJsonObject &json)
{
    Vod *vod = new Vod();

    if (!json["_id"].isNull())
        vod->setId(json["_id"].toString());

    if (!json["preview"].isNull()) {
        const QJsonValue & preview = json["preview"];
        if (preview.isString()) {
            vod->setPreview(preview.toString());
        }
        else if (preview.isObject()) {
            const QJsonValue & previewUrl = preview.toObject()["medium"];
            if (previewUrl.isString()) {
                vod->setPreview(previewUrl.toString());
            }
        }
    }

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

PagedResult<Channel*> JsonParser::parseChannels(const QByteArray &data)
{
    PagedResult<Channel*> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        QJsonArray arr = json["channels"].toArray();
        foreach (const QJsonValue &item, arr){
            out.items.append(JsonParser::parseChannelJson(item.toObject()));
        }

        out.total = json["_total"].toInt();
    }

    return out;
}

PagedResult<Channel *> JsonParser::parseFavourites(const QByteArray &data)
{
    PagedResult<Channel *> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);
    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();

        out.total = json["_total"].toInt();

        QJsonArray arr = json["follows"].toArray();
        foreach (const QJsonValue &item, arr){
            out.items.append(JsonParser::parseChannelJson(item.toObject()["channel"].toObject()));
        }
    }

    return out;
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
                channels.append(JsonParser::parseStreamJson(item.toObject()["stream"].toObject(), true));
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
                + QString("&token=") + QUrl::toPercentEncoding(tokenData)
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

QPair<QString, quint64> JsonParser::parseUser(const QByteArray &data)
{
    QString displayName;
    quint64 userId = 0;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data,&error);

    if (error.error == QJsonParseError::NoError){
        QJsonObject json = doc.object();
        if (!json["name"].isNull())
            displayName = json["name"].toString();
        userId = json["_id"].toString().toULongLong();
    }

    return qMakePair(displayName, userId);
}

QList<quint64> JsonParser::parseUsers(const QByteArray &data)
{
    QList<quint64> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();
        for (const auto & user : json["users"].toArray()) {
            auto userId = user.toObject()["_id"];
            if (userId.isDouble()) {
                out.append(static_cast<quint64>(userId.toDouble()));
            }
            else {
                out.append(userId.toString().toULongLong());
            }
        }
    }

    return out;
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

void JsonParser::parseBitsData(const QByteArray &data, QMap<QString, QMap<QString, QString>> & outUrls, QMap<QString, QMap<QString, QString>> & outColors)
{
    const QString BITS_THEME = "dark";
    const QString BITS_TYPE = "animated";
    const QString BITS_SIZE_LODPI = "1";
    const QString BITS_SIZE_HIDPI = "2";

    const QString BITS_SIZE = SettingsManager::getInstance()->hiDpi() ? BITS_SIZE_HIDPI : BITS_SIZE_LODPI;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        auto actions = json["actions"].toArray();
        for (const auto & actionEntry : actions) {
            
            QMap<QString, QString> actionUrlsMap;
            QMap<QString, QString> actionColorsMap;

            const QJsonObject & actionObj = actionEntry.toObject();
            QString actionPrefix = actionObj["prefix"].toString();

            const QJsonArray & tiers = actionObj["tiers"].toArray();
            for (const auto & tierEntry : tiers) {
                const QJsonObject & tierObj = tierEntry.toObject();

                int minBits = tierObj["min_bits"].toInt();

                const QString & url = tierObj["images"].toObject()[BITS_THEME].toObject()[BITS_TYPE].toObject()[BITS_SIZE].toString();
                qDebug() << "bits url for" << actionPrefix << "minBits" << minBits << "is" << url;
                actionUrlsMap.insert(QString::number(minBits), url);

                const QString & color = tierObj["color"].toString();
                actionColorsMap.insert(QString::number(minBits), color);
            }

            if (actionUrlsMap.size() > 0) {
                outUrls.insert(actionPrefix, actionUrlsMap);
            }

            if (actionColorsMap.size() > 0) {
                outColors.insert(actionPrefix, actionColorsMap);
            }
        }
    }
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

int unicodeLen(const QString & text) {
    int out = 0;
    for (auto i = 0; i < text.length(); i++) {
        auto ch = text.at(i);
        if ((ch < 0xd800) || (ch > 0xdbff)) {
            ++out;
        }
    }
    return out;
}

ReplayChatMessage parseVodChatEntry(const QJsonValue &entry) {
    ReplayChatMessage out;
    
    const QJsonObject & attributes = entry.toObject();

    auto msgId = attributes["_id"].toString();
    out.id = msgId;

    auto commenter = attributes["commenter"].toObject();

    auto name = commenter["name"].toString();
    out.from = name;
    QString state = attributes["state"].toString();
    bool deleted = state != QString("published");
    out.deleted = deleted; // XXX other types to take as valid?

    auto message = attributes["message"].toObject();
    out.message = message["body"].toString();
    auto channelId = attributes["channel_id"].toString();
    out.room = channelId;
    out.videoOffset = attributes["content_offset_seconds"].toDouble() * 1000.0;
    out.timestamp = out.videoOffset;

    auto source = attributes["source"].toString();
    if (source == QString("chat")) {
        out.command = "PRIVMSG";
    }
    else {
        // XXX need some more guesses for these
        qDebug() << "unknown message source" << source; // XXX remove
        out.command = "PRIVMSG";
    }

    int unicodePos = 0;
    auto fragments = message["fragments"].toArray();

    QSet<int> emoteIdsSeen;

    for (const auto & fragment : fragments) {
        auto fragmentObj = fragment.toObject();
        auto text = fragmentObj["text"].toString();

        auto curTextUnicodeLen = unicodeLen(text);

        if (!fragmentObj["emoticon"].isNull()) {
            auto emoteObj = fragmentObj["emoticon"].toObject();

            QString emotioconIDStr = emoteObj["emoticon_id"].toString();
            int first = unicodePos;
            int last = unicodePos + curTextUnicodeLen - 1; // XXX off by one?
            int emoteId = emotioconIDStr.toInt();
            if (emoteIdsSeen.constFind(emoteId) == emoteIdsSeen.constEnd()) {
                emoteIdsSeen.insert(emoteId);
                out.emoteList.append(emoteId);
            }
            out.emotePositionsMap.insert(first, qMakePair(last, emoteId));
        }

        unicodePos += curTextUnicodeLen;

    }
    
    // XXX tags collection stuff not hooked up yet
    // system-msg

    // @badges=staff/1,broadcaster/1,turbo/1;color=#008000;display-name=TWITCH_UserName;emotes=;mod=0;msg-id=resub;msg-param-months=6;room-id=1337;subscriber=1;system-msg=TWITCH_UserName\shas\ssubscribed\sfor\s6\smonths!;login=twitch_username;turbo=1;user-id=1337;user-type=staff :tmi.twitch.tv USERNOTICE #channel :Great stream -- keep it up!
    QList<QString> badges;
    for (const auto & badge : message["user_badges"].toArray()) {
        auto badgeObj = badge.toObject();
        auto badgeId = badgeObj["_id"].toString();
        badges.append(badgeId + QString("/") + badgeObj["version"].toString());

        // not sure if anything in the front end needs these as tags if we added the badges directly
        if (badgeId == QString("moderator")) {
            out.tags.insert("mod", true);
        }

        if (badgeId == QString("subscriber")) {
            out.tags.insert("subscriber", true);
        }

        if (badgeId == QString("turbo")) {
            out.tags.insert("turbo", true);
        }
    }

    if (!badges.isEmpty()) {
        out.tags.insert("badges", badges.join(","));
    }

    // most of this stuff is being populated for backward compatibility with things that currently operate on the tags
    // and can be taken out if nothing uses it
    out.tags.insert("msg-id", msgId);
    out.tags.insert("display-name", commenter["display_name"].toString());
    out.tags.insert("login", name);
    out.tags.insert("color", message["user_color"].toString());
    out.tags.insert("room-id", channelId);
    out.tags.insert("user-id", commenter["_id"].toString());
    out.tags.insert("user-type", commenter["type"].toString());
        
    return out;
}

ReplayChatPiece JsonParser::parseVodChatPiece(const QByteArray &data)
{
    ReplayChatPiece out;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        if (!json["comments"].isNull() && json["comments"].isArray()) {
            const QJsonArray & chatEntries = json["comments"].toArray();
            for (const auto & entry : chatEntries) {
                out.comments.append(parseVodChatEntry(entry));
            }
        }

        out.next = json["_next"].toString();
        out.prev = json["_prev"].toString();
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

PagedResult<QString> JsonParser::parseBlockList(const QByteArray &data)
{
    PagedResult<QString> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        out.total = json["_total"].toInt();

        QJsonArray blocks = json["blocks"].toArray();

        for (const auto & block : blocks) {
            const auto & blockObj = block.toObject();
            const auto & name = blockObj["user"].toObject()["name"].toString();
            if (!name.isEmpty()) {
                out.items.append(name);
            }
        }
    }

    return out;
}


QMap<QString, QString> JsonParser::parseBttvEmotesData(const QByteArray &data)
{
    QMap<QString, QString> out;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();

        QJsonArray emotes = json["emotes"].toArray();

        for (const auto & emote : emotes) {
            const auto & emoteObj = emote.toObject();
            const auto & id = emoteObj["id"].toString();
            const auto & code = emoteObj["code"].toString();
            if (!id.isEmpty() && !code.isEmpty()) {
                out.insert(code, id);
            }
        }
    }

    return out;
}

QPair<QString,QString> JsonParser::parseVersion(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    QString version;
    QString url;

    if (error.error == QJsonParseError::NoError) {
        QJsonObject json = doc.object();
        version = json["name"].toString();
        url = json["html_url"].toString();
    }

    return qMakePair<QString,QString>(version, url);
}
