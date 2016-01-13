#include "channelmanager.h"
#include <QStringRef>
#include <QDir>
#include <QProcess>
#include "../util/fileutils.h"
#include <QThread>
#include <QJsonArray>

QSortFilterProxyModel *ChannelManager::getFeaturedProxy() const
{
    return featuredProxy;
}

quint16 ChannelManager::getCache() const
{
    return cache;
}

bool ChannelManager::isAlert() const
{
    return alert;
}

ChannelManager::ChannelManager(){
    alert = true;

    cache = 10;

    netman = new NetworkManager(this);

    favouritesModel = new ChannelListModel();

    resultsModel = new ChannelListModel();

    featuredModel = new ChannelListModel();

    gamesModel = new GameListModel();

    connect(favouritesModel,SIGNAL(channelOnlineStateChanged(Channel*)),this,SLOT(notify(Channel*)));

    favouritesProxy = new QSortFilterProxyModel();
    favouritesProxy->setSourceModel(favouritesModel);
    favouritesProxy->setSortRole(ChannelListModel::Roles::ViewersRole);
    favouritesProxy->sort(0, Qt::DescendingOrder);

    featuredProxy = new QSortFilterProxyModel();
    featuredProxy->setSourceModel(featuredModel);
    featuredProxy->setSortRole(ChannelListModel::Roles::ViewersRole);
    featuredProxy->sort(0, Qt::DescendingOrder);
}

ChannelManager::~ChannelManager(){
    qDebug() << "Destroyer: ChannelManager";

    delete netman;

    save();
    delete favouritesModel;
    delete resultsModel;
    delete featuredModel;
    delete gamesModel;
    delete favouritesProxy;
    delete featuredProxy;
}

ChannelListModel *ChannelManager::getFavouritesModel() const
{
    return favouritesModel;
}

QSortFilterProxyModel *ChannelManager::getFavouritesProxy() const
{
    return favouritesProxy;
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

    if (!json["alert"].isNull()){
        alert = json["alert"].toBool();
    }

    if (!json["cache"].isNull()){
        cache = json["cache"].toInt();
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

        Channel* channel = new Channel(
                    obj["uri"].toString(),
                    obj["title"].toString(),
                    obj["info"].toString(),
                    obj["alert"].toBool(),
                    obj["lastSeen"].toInt(),
                    obj["logo"].toString(),
                    obj["preview"].toString());
        channel->setId(obj["id"].toInt());

        favouritesModel->addChannel(channel);

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

    obj["alert"] = QJsonValue(alert);

    obj["cache"] = QJsonValue(cache);

    return util::writeFile(DATA_FILE,QJsonDocument(obj).toJson());
}

void ChannelManager::addToFavourites(const quint32 &id){
    Channel *channel = resultsModel->find(id);

    if (!channel){
        channel = featuredModel->find(id);
    }

    if (channel){
        favouritesModel->addChannel(new Channel(*channel));
        channel->setFavourite(true);
        resultsModel->updateChannelForView(channel);

        //Update featured also
        featuredModel->updateChannelForView(channel);
    }
}

Channel* ChannelManager::find(const QString &q){
    return favouritesModel->find(q);
}

void ChannelManager::removeFromFavourites(const quint32 &id){
    favouritesModel->removeChannel(favouritesModel->find(id));

    //Update results
    Channel* channel = resultsModel->find(id);
    if (channel){
        channel->setFavourite(false);
        resultsModel->updateChannelForView(channel);
    }

    //Update featured
    channel = featuredModel->find(id);
    if (channel){
        channel->setFavourite(false);
        featuredModel->updateChannelForView(channel);
    }
}

void ChannelManager::play(const QString &url){
    if (QFile::exists(PLAY_FILE)){

        if (!QFileInfo(PLAY_FILE).isExecutable())
            QProcess::execute("chmod +x " + QString(PLAY_FILE));

        QStringList args;
        args << url;

        QProcess::startDetached(PLAY_FILE, args);
    }
    else qDebug() << "Couldn't locate 'play.sh'";
}

void ChannelManager::checkStreams(const QList<Channel *> &list)
{
    if (list.size() == 0)
        return;

    int c_index = 0;
    QString channelsUrl = "";

    foreach(Channel* channel, list){
        if (c_index++ > 0)
            channelsUrl += ",";
        channelsUrl += channel->getServiceName();

        if (c_index >= list.size() || c_index >= 50){
            QString url = KRAKEN_API
                    + QString("/streams?limit=%1&channel=").arg(c_index)
                    + channelsUrl;

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
        if (favouritesModel->find(channel->getId()))
            channel->setFavourite(true);

        resultsModel->addChannel(new Channel(*channel));

        if (!channel->isOnline())
            needsStreamCheck = true;
    }

    if (needsStreamCheck)
        checkStreams(list);

    qDeleteAll(list);

    emit resultsUpdated();
}

void ChannelManager::getFeatured()
{
    featuredModel->clear();

    netman->getFeaturedStreams();
}

void ChannelManager::findPlaybackStream(const QString &serviceName)
{
    netman->getChannelPlaybackStream(serviceName);
}

void ChannelManager::setCache(const quint16 &secs)
{
    cache = secs;
    emit cacheUpdated();
}

void ChannelManager::setAlert(const bool &val)
{
    alert = val;
}

void ChannelManager::addFeaturedResults(const QList<Channel *> &list)
{
    foreach (Channel *channel, list){
        if (favouritesModel->find(channel->getId()))
            channel->setFavourite(true);

        featuredModel->addChannel(new Channel(*channel));
    }

    qDeleteAll(list);

    emit featuredUpdated();
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

void ChannelManager::notify(Channel *channel)
{
#ifdef ENABLE_NOTIFY
    if (alert)
        notif.notify(channel->getName() + (channel->isOnline() ? " is now streaming" : " has gone offline"),
                  channel->getInfo(),
                  channel->getLogourl());
#endif
}
