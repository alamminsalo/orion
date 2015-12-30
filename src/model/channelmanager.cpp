#include "channelmanager.h"

ChannelManager::ChannelManager(){
    netman = new NetworkManager(this);
    connect(this,SIGNAL(channelStateChanged(Channel*)),this,SLOT(notify(Channel*)));
}

ChannelManager::~ChannelManager(){
    qDebug() << "Destroyer: ChannelManager";
    delete netman;
    save();
    clearData();
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
    if (!QFile::exists(PLAY_FILE)){
        netman->getFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/scripts/play.sh",PLAY_FILE);
    }
    if (!QFile::exists(DIALOG_FILE)){
        netman->getFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/scripts/dialog.sh",DIALOG_FILE);
    }
    if (!QFile::exists("resources/logos/default.png")){
        qDebug() << "default channel logo not found, fetching...";
        netman->getFile(DEFAULT_LOGO_URL,"resources/logos/default.png");
    }
}

QVariantList ChannelManager::getChannelsList()
{
    QVariantList list;

    qSort(this->channels.begin(), this->channels.end(), Channel::greaterThan);

    foreach (Channel* channel, channels){
        list.append(QVariant::fromValue(channel));
    }

    return list;
}

QVariantList ChannelManager::getGamesList()
{
    QVariantList list;

    foreach (Game* game, games){
        list.append(QVariant::fromValue(game));
    }

    return list;
}

QVariantList ChannelManager::getResultsList()
{
    QVariantList list;

    foreach (Channel* channel, results){
        list.append(QVariant::fromValue(channel));
    }

    return list;
}

void ChannelManager::notify(Channel *channel)
{
#ifdef Q_OS_LINUX
    if (QFile::exists(DIALOG_FILE)){
        if (!QFileInfo(DIALOG_FILE).isExecutable())
            QProcess::execute("chmod +x " + QString(DIALOG_FILE));

        QStringList args;
        args << channel->getName() + (channel->isOnline() ? " is now streaming" : " has gone offline")
             << channel->getInfo()
             << "/" + channel->getLogoPath();

        QProcess::startDetached(DIALOG_FILE, args);
    }
#endif
    channelsChanged = true;
}

void ChannelManager::load(const QString &path){
    readJSON(path);
}

void ChannelManager::load(){
    readJSON(DATAURI);
}

void ChannelManager::save()
{
    writeJSON(DATAURI);
}

bool ChannelManager::readJSON(const QString &filename){

    qDebug() << "Opening JSON...";

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

    qDebug() << arr.size();

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
        qDebug() << "Added channel " << obj["title"].toString();
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
        netman->getStream(channel->getUriName());
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

void ChannelManager::checkChannels(){
	for (unsigned int i=0; i<channels.size(); i++){
        netman->getChannel(channels.at(i)->getUriName());
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
    qDeleteAll(channels);
    //qDeleteAll(games);
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

    int c_index = 0;
    QString channelsUrl = "";

    for(size_t i = 0; i < channels.size(); i++){
        Channel* channel = channels.at(i);
        if (c_index++ > 0)
            channelsUrl += ",";
        channelsUrl += channel->getUriName();

        if (c_index >= channels.size() || c_index >= 50){
            QString url = TWITCH_URI
                    + QString("/streams?limit=%1&channel=").arg(c_index)
                    + channelsUrl;

            qDebug() << url;

            netman->getAllStreams(url);

            channelsUrl = "";
            c_index = 0;
        }
    }
}

void ChannelManager::getGames()
{
    netman->getGames(25,games.size());
}

void ChannelManager::searchChannels(const QString &q, const quint32 &offset, const quint32 &limit, bool clear)
{
    if (clear)
        qDeleteAll(results);

    netman->searchChannels(q, offset, limit);
}

QList<Channel *> ChannelManager::getResults() const
{
    return results;
}

void ChannelManager::setResults(const QList<Channel *> &value)
{
    results = value;

    emit resultsUpdated();
}

void ChannelManager::updateChannels(const QList<Channel*> &list, const QList<Channels*> channels)
{
    foreach(Channel *channel, list){
        updateChannel(channel, channels);
    }
    qDeleteAll(list);

    emit channelsUpdated();
}

void ChannelManager::updateChannel(Channel *item, const QList<Channels*> channels)
{
    if (!item->getUriName().isEmpty()){

        Channel *channel = find(item->getUriName());

        if (channel){
            channel->setName(item->getName());

            if (!item->getLogourl().isEmpty()){
                channel->setLogourl(item->getLogourl());
            } else {
                channel->setLogourl(DEFAULT_LOGO_URL);
            }
            channel->setLogoPath(item->getLogoPath());
            channel->setInfo(item->getInfo());
            channel->updated();
        }
    }
}

void ChannelManager::updateStreams(const QList<Channel*> &list)
{
    //Online channels
    foreach(Channel *channel, list){
        updateStream(channel);
    }
    qDeleteAll(list);

    emit channelsUpdated();
}

void ChannelManager::updateStream(Channel *item)
{
    if (!item->getUriName().isEmpty()){

        Channel *channel = find(item->getUriName());

        if (channel){
                if (item->isOnline()){
                    channel->setViewers(item->getViewers());
                    channel->setGame(item->getGame());
                    channel->setPreviewurl(item->getPreviewurl());
                    channel->setPreviewPath(item->getPreviewPath());

                    if (!item->getName().isEmpty()){
                        updateChannel(item);
                    }
                } else if (channel->getName().isEmpty()){
                    netman->getChannel(channel->getUriName());
                }

                if (channel->isOnline() != item->isOnline()){
                    channel->setOnline(item->isOnline());
                    emit channelStateChanged(channel);
                    channel->updated();
                }
        }
    }
}

void ChannelManager::updateGames(const QList<Game*> &list)
{
    foreach(Game* game, list){
        games.append(new Game(*game));
    }

    qDeleteAll(list);

    emit gamesUpdated();
}
