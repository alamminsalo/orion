#include "channelmanager.h"


ChannelManager::ChannelManager(){
    netman = new NetworkManager(this);
}

void ChannelManager::checkResources()
{
    if (!QDir().exists("resources")){
        qDebug() << "dir \"resources\" not found, making...";
        QDir().mkdir("resources");
    }
    if (!QFile::exists("resources/icon.svg")){
        qDebug() << "logo file not found, fetching...";
        netman->getFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/icon.svg","resources/icon.svg");
    }
    if (!QDir().exists("resources/preview")){
        qDebug() << "dir \"preview\" not found, making...";
        QDir().mkdir("resources/preview");
    }
    if (!QFile::exists("resources/preview/offline.png")){
        qDebug() << "offline.png not found, fetching...";
        netman->getFile("https://raw.githubusercontent.com/alamminsalo/kstream/dev/resources/preview/offline.png","resources/preview/offline.png");
    }
    if (!QDir().exists("resources/logos")){
        qDebug() << "dir \"logos\" not found, making...";
        QDir().mkdir("resources/logos");
    }
    if (!QDir().exists("resources/scripts")){
        qDebug() << "dir \"scripts\" not found, making...";
        QDir().mkdir("resources/scripts");
    }
    if (!QFile::exists("resources/scripts/play.sh")){
        netman->getFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/scripts/play.sh","resources/scripts/play.sh");
#ifndef Q_OS_WIN
        QProcess::startDetached("chmod +x resources/scripts/play.sh");
#endif
    }
    if (!QFile::exists("resources/scripts/dialog.sh")){
        netman->getFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/scripts/dialog.sh","resources/scripts/dialog.sh");
#ifndef Q_OS_WIN
        QProcess::startDetached("chmod +x resources/scripts/dialog.sh");
#endif
    }
    if (!QFile::exists("resources/logos/default.png")){
        qDebug() << "default channel logo not found, fetching...";
        netman->getFile(DEFAULT_LOGO_URL,"resources/logos/default.png");
    }
}

ChannelManager::~ChannelManager(){
    qDebug() << "Destroyer: ChannelManager";
    delete netman;
    save();
    clearData();
}

void ChannelManager::load(){
    readJSON(DATAURI);
}

void ChannelManager::save()
{
    writeJSON(DATAURI);
}

bool ChannelManager::readJSON(const QString &filename){
    QJsonParseError error;
    if (!QFile::exists(filename)){
        qDebug() << "File doesn't exist";
        return false;
    }
    QJsonObject json = QJsonDocument::fromJson(util::readFile(filename).toLocal8Bit(),&error).object();

    if (error.error != QJsonParseError::NoError){
        qDebug() << "Parsing error!";
        return false;
    }

    if (json["channels"].isUndefined()){
        qDebug() << "Error: Bad file format: Missing field \"channels\"";
        return false;
	}

    if (!json["channels"].isArray()){
        qDebug() << "Error: Bad file format: channels is not array";
        return false;
    }

    const QJsonArray &arr = json["channels"].toArray();

    foreach(const QJsonValue &value, arr){
        QJsonObject obj = value.toObject();

        if (obj["title"].isUndefined() || obj["title"].isNull()){
            qDebug() << "title is missing";
        }
        if (obj["uri"].isUndefined() || obj["uri"].isNull()){
            qDebug() << "uri is missing";
        }
        if (obj["info"].isUndefined() || obj["info"].isNull()){
            qDebug() << "info is missing";
        }
        if (obj["alert"].isUndefined() || obj["alert"].isNull()){
            qDebug() << "alert is missing";
        }
        if (obj["lastSeen"].isUndefined() || obj["lastSeen"].isNull()){
            qDebug() << "lastSeen is missing";
        }
        if (obj["logo"].isUndefined() && obj["logo"].isNull()){
            qDebug() << "logo is missing";
        }
        if (obj["preview"].isUndefined() && obj["preview"].isNull()){
            qDebug() << "preview is missing";
        }

        channels.push_back(
                    new Channel(
                        obj["uri"].toString(),
                        obj["title"].toString(),
                        obj["info"].toString(),
                        obj["alert"].toBool(),
                        obj["lastSeen"].toInt(),
                        obj["logo"].toString(),
                        obj["preview"].toString()
                               )
                    );
    }

	return true;
}

bool ChannelManager::writeJSON(const QString& path){
    QJsonArray arr;

    foreach (Channel* channel, channels){
        arr.append(QJsonValue(channel->getJSON()));
    }

    QJsonValue val(arr);
    QJsonObject obj;
    obj["channels"] = val;

    return util::writeFile(path,QJsonDocument(obj).toJson());
}

void ChannelManager::add(Channel *channel){
    channels.push_back(channel);
    emit newChannel(channel);
}

void ChannelManager::add(const char* title, const char* uri, const char *info, const char *alert){
    add(new Channel(title,uri,info,alert));
}

void ChannelManager::add(const QString &uriName){
    Channel *channel = find(uriName);
    if (channel){
        emit channelExists(channel);
    }
    else{
        channel = new Channel(uriName);
        add(channel);
        netman->getStream(channel);
    }
}

Channel *ChannelManager::find(const QString &uriName){
    foreach (Channel *channel, channels){
        if (channel->getUriName() == uriName)
            return channel;
	}
	return 0;
}

int ChannelManager::findPos(const QString &uriName){
	for (unsigned int i=0; i<channels.size(); i++){
        if (channels.at(i)->getUriName() == uriName)
			return i;
	}
	return -1;
}

void ChannelManager::updateChannels(){
	for (unsigned int i=0; i<channels.size(); i++){
        netman->getChannelData(channels.at(i));
	}
}

void ChannelManager::remove(const char *channelName){	
	int i = findPos(channelName);
    if (i != -1){
        delete channels.at(i);
		channels.erase(channels.begin() + i);
    }
}

void ChannelManager::remove(Channel* channel){
    for (unsigned int i=0; i < channels.size(); i++){
        if (channels[i] == channel){
            delete channel;
            channels.erase(channels.begin() + i);
        }
    }
}

void ChannelManager::clearData(){
    for (unsigned int i = 0; i < channels.size(); i++){
        delete channels.at(i);
	}
    channels.clear();
}

void ChannelManager::play(Channel* channel){
    if (QFile::exists("resources/scripts/play.sh")){
        QString cmd = "resources/scripts/play.sh "+channel->getFullUri();
        QProcess::startDetached(cmd);
    }
    else qDebug() << "Couldn't locate 'play.sh'";
}

void ChannelManager::checkStreams()
{
    if (channels.size() == 0)
        return;

    QString url = TWITCH_URI;
    url += "/streams?limit=100&channel=";
    for(size_t i = 0; i < channels.size(); i++){
        Channel* channel = channels.at(i);
        if (i > 0)
            url += ",";
        url += channel->getUriName();
        channel->setChanged(false);
    }

    netman->getAllStreams(url);
}

void ChannelManager::parseStreams(const QJsonObject &json)
{
    // PARSE ONLINE CHANNELS
    QJsonArray arr = json["streams"].toArray();
    foreach (const QJsonValue &item, arr){
        parseStream(item.toObject());
    }

    //PARSE OFFLINE CHANNELS
    foreach (Channel *channel, channels){
        if (!channel->isChanged()){

            //If missing display name or channel logo data, update channel data
            if (channel->getName().isEmpty() || !QFile::exists(channel->getLogoPath())){
                netman->getChannelData(channel);
            }

            if (channel->isOnline()){
                channel->setOnline(false);
                emit channelStateChanged(channel);
            }
        }
    }
}

void ChannelManager::parseStream(const QJsonObject item, Channel* channel){

    if (item["channel"].isNull()){
        return;
    }
    QJsonObject channelValue = item["channel"].toObject();

    if (!channel){
        channel = find(channelValue["name"].toString());
    }

    if (channel){

        if (!channelValue["display_name"].isNull()){
            channel->setName(channelValue["display_name"].toString());
        }

        if (!channelValue["logo"].isNull()){
            QString logouri = channelValue["logo"].toString();
            QStringRef extension(&logouri,logouri.lastIndexOf("."),(logouri.length() - logouri.lastIndexOf(".")));
            QString logopath = "resources/logos/";

            logopath += channel->getUriName();
            logopath += extension;

            channel->setLogourl(logouri);
            channel->setLogoPath(logopath);

            if (!QFile::exists(logopath)){
                netman->getLogo(channel);
            }
        } else {
            channel->setLogoPath("resources/logos/default.png");
            channel->iconUpdated();
        }

        if (!item["viewers"].isNull()){
            channel->setViewers(item["viewers"].toInt());
        }

        if (!item["game"].isNull()){
            channel->setGame(item["game"].toString());
        }

        if (!channelValue["status"].isNull()){
            channel->setInfo(channelValue["status"].toString());
        }

        if (!item["preview"].isNull()){

            QJsonObject previewValue = item["preview"].toObject();

            if (!previewValue["medium"].isNull()){
                QString previewuri = previewValue["medium"].toString();
                QStringRef extension(&previewuri,previewuri.lastIndexOf("."),(previewuri.length() - previewuri.lastIndexOf(".")));
                QString previewpath = "resources/preview/";
                previewpath += channel->getUriName();
                previewpath += extension;

                channel->setPreviewurl(previewuri);
                channel->setPreviewPath(previewpath);

                netman->getFile(channel->getPreviewurl(),channel->getPreviewPath());
            }
        }

        if (!channel->isOnline()){
            channel->setOnline(true);
            emit channelStateChanged(channel);
        }
        else {
            channel->updated();
        }
        channel->setChanged(true);
    }
}

void ChannelManager::parseChannel(const QJsonObject item, Channel *channel){
    if (item["name"].isNull()){
        return;
    }

    if (channel){

        qDebug() << "Parsing channel data for " <<  channel->getUriName();

        if (!item["display_name"].isNull()){
            channel->setName(item["display_name"].toString());
        }

        if (!item["status"].isNull()){
            channel->setInfo(item["status"].toString());
        }

        if (!item["logo"].isNull()){
            QString logouri = item["logo"].toString();
            QStringRef extension(&logouri,logouri.lastIndexOf("."),(logouri.length() - logouri.lastIndexOf(".")));
            QString logopath = "resources/logos/";

            logopath += channel->getUriName();
            logopath += extension;

            channel->setLogourl(logouri);
            channel->setLogoPath(logopath);

            if (!QFile::exists(logopath)){
                netman->getLogo(channel);
            }
        } else {
            channel->setLogoPath("resources/logos/default.png");
            channel->iconUpdated();
        }
        channel->updated();
    }
}

