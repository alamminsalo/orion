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
            channels.append(JsonParser::parseStream(item.toObject()));
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
            foreach (Channel* channel, channels){
                cnames.removeOne(channel->getServiceName());
            }
            foreach(QString name, cnames){
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
        return parseStream(doc.object());
    }
    return new Channel();
}

Channel* JsonParser::parseStream(const QJsonObject &json)
{
    Channel* channel = new Channel();

    //Take name from _links
    QJsonObject links = json["_links"].toObject();
    if (!links["self"].isNull()){
        QString channelName = links["self"].toString();
        channelName.remove(0, channelName.lastIndexOf('/') + 1);
        channel->setServiceName(channelName);
    }

    if (!json["preview"].isNull()){

        QJsonObject preview = json["preview"].toObject();

        if (!preview["large"].isNull()){
            channel->setPreviewurl(preview["large"].toString());
        }
    }

    if (!json["viewers"].isNull()){
        channel->setViewers(json["viewers"].toInt());
    }

    if (!json["game"].isNull()){
        channel->setGame(json["game"].toString());
    }

    if (!json["channel"].isNull()){

        Channel *c = parseChannel(json["channel"].toObject());
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
        return parseChannel(doc.object());
    }
    return new Channel();
}

Channel* JsonParser::parseChannel(const QJsonObject &json)
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
            channels.append(JsonParser::parseChannel(item.toObject()));
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
                channels.append(JsonParser::parseStream(item.toObject()["stream"].toObject()));
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
                  "&$allow_audio_only=true";
    }

    return url;
}
