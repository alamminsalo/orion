#include "channelmanager.h"

ChannelManager::ChannelManager(){
    netman = new NetworkManager(this);

    favouritesModel = new ChannelListModel();

    resultsModel = new ChannelListModel();

    gamesModel = new GameListModel();

    connect(favouritesModel,SIGNAL(channelOnlineStateChanged(Channel*)),this,SLOT(notify(Channel*)));

    favouritesProxy = new QSortFilterProxyModel();
    favouritesProxy->setSourceModel(favouritesModel);
    favouritesProxy->setSortRole(ChannelListModel::Roles::ViewersRole);
    favouritesProxy->sort(0, Qt::DescendingOrder);

//    resultsProxy = new QSortFilterProxyModel();
//    resultsProxy->setSourceModel(resultsModel);
//    resultsProxy->setSortRole(ChannelListModel::Roles::ViewersRole);
//    resultsProxy->sort(0, Qt::DescendingOrder);
}

ChannelManager::~ChannelManager(){
    qDebug() << "Destroyer: ChannelManager";

    delete netman;

    save();
    delete favouritesModel;
    delete resultsModel;
    delete gamesModel;
}

ChannelListModel *ChannelManager::getFavouritesModel() const
{
    return favouritesModel;
}

QSortFilterProxyModel *ChannelManager::getFavouritesProxy() const
{
    return favouritesProxy;
}

QSortFilterProxyModel *ChannelManager::getResultsProxy() const
{
    return resultsProxy;
}

GameListModel *ChannelManager::getGamesModel() const
{
    return gamesModel;
}

ChannelListModel *ChannelManager::getResultsModel() const
{
    return resultsModel;
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
    if (!QFile::exists(DATA_FILE)){
        QFile file(DATA_FILE);
        file.open(QIODevice::ReadWrite);
        file.write("{}");
    }
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
}

bool ChannelManager::load(){

    qDebug() << "Opening JSON...";

    QJsonParseError error;
    if (!QFile::exists(DATA_FILE)){
        qDebug() << "File doesn't exist";
        return false;
    }
    QJsonObject json = QJsonDocument::fromJson(util::readFile(DATA_FILE).toLocal8Bit(),&error).object();

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

        favouritesModel->addChannel(
                new Channel(
                    obj["uri"].toString(),
                    obj["title"].toString(),
                    obj["info"].toString(),
                    obj["alert"].toBool(),
                    obj["lastSeen"].toInt(),
                    obj["logo"].toString(),
                    obj["preview"].toString()
                ));
        qDebug() << "Added channel " << obj["title"].toString();
    }

	return true;
}

bool ChannelManager::save() const
{
    QJsonArray arr;

    foreach (Channel* channel, favouritesModel->getChannels()){
        arr.append(QJsonValue(channel->getJSON()));
    }

    QJsonValue val(arr);
    QJsonObject obj;
    obj["channels"] = val;

    return util::writeFile(DATA_FILE,QJsonDocument(obj).toJson());
}

void ChannelManager::addToFavourites(const QString &serviceName){
    Channel *channel = resultsModel->find(serviceName);
    if (channel){
        favouritesModel->addChannel(new Channel(*channel));
    }
}

Channel* ChannelManager::find(const QString &q){
    return favouritesModel->find(q);
}

void ChannelManager::removeFromFavourites(const QString &name){
    favouritesModel->removeChannel(favouritesModel->find(name));
}

void ChannelManager::play(const QString &url){
    if (QFile::exists("resources/scripts/play.sh")){
        QString cmd = "resources/scripts/play.sh "+url;
        QProcess::startDetached(cmd);
    }
    else qDebug() << "Couldn't locate 'play.sh'";
}

void ChannelManager::checkStreams(const QList<Channel *> &list)
{
    if (list.size() == 0)
        return;

    int c_index = 0;
    QString channelsUrl = "";

    for(size_t i = 0; i < list.size(); i++){
        Channel* channel = list.at(i);
        if (c_index++ > 0)
            channelsUrl += ",";
        channelsUrl += channel->getServiceName();

        if (c_index >= list.size() || c_index >= 50){
            QString url = TWITCH_URI
                    + QString("/streams?limit=%1&channel=").arg(c_index)
                    + channelsUrl;

            qDebug() << url;

            netman->getStreams(url);

            channelsUrl = "";
            c_index = 0;
        }
    }
}

void ChannelManager::checkFavourites()
{
    checkStreams(favouritesModel->getChannels());
}

void ChannelManager::getGames(const quint32 &offset, const quint32 &limit, bool clear)
{
    if (clear){
        gamesModel->clear();
    }

    netman->getGames(offset,limit);
}

void ChannelManager::searchChannels(QString q, const quint32 &offset, const quint32 &limit, bool clear)
{
    if (clear)
        resultsModel->clear();

    if (q.startsWith(":game ")){
        q.replace(":game ", "");
        netman->getStreamsForGame(q, offset, limit);

    } else {
        netman->searchChannels(q, offset, limit);
    }

    emit searchingStarted();
}

void ChannelManager::addSearchResults(const QList<Channel*> &list)
{
    bool needsStreamCheck = false;

    foreach (Channel *channel, list){
        resultsModel->addChannel(new Channel(*channel));

        if (!channel->isOnline())
            needsStreamCheck = true;
    }

    if (needsStreamCheck)
        checkStreams(list);

    qDeleteAll(list);

    emit resultsUpdated();
}

void ChannelManager::updateFavourites(const QList<Channel*> &list)
{
    favouritesModel->updateChannels(list);
    qDeleteAll(list);
}

//Updates channel streams in all models
void ChannelManager::updateStreams(const QList<Channel*> &list)
{
    favouritesModel->updateStreams(list);
    resultsModel->updateStreams(list);
    qDeleteAll(list);
}

void ChannelManager::updateGames(const QList<Game*> &list)
{
    foreach(Game* game, list){
        gamesModel->addGame(new Game(*game));
    }

    qDeleteAll(list);

    emit gamesUpdated();
}
