#include "channelmanager.h"
#include <QStringRef>
#include <QDir>
#include <QProcess>
#include "../util/fileutils.h"
#include <QThread>
#include <QJsonArray>
#include <QApplication>

QString appPath(){
    return qApp->applicationDirPath() + "/" + DATA_FILE;
}

QSortFilterProxyModel *ChannelManager::getFeaturedProxy() const
{
    return featuredProxy;
}

bool ChannelManager::isAlert() const
{
    return alert;
}

int ChannelManager::getAlertPosition() const
{
    return alertPosition;
}

void ChannelManager::setAlertPosition(const int &value)
{
    alertPosition = value;
}

void ChannelManager::addToFavourites(const quint32 &id, const QString &serviceName, const QString &title,
                                     const QString &info, const QString &logo, const QString &preview,
                                     const QString &game, const qint32 &viewers, bool online)
{
    if (!favouritesModel->find(id)){
        Channel *channel = new Channel();
        channel->setId(id);
        channel->setServiceName(serviceName);
        channel->setName(title);
        channel->setInfo(info);
        channel->setLogourl(logo);
        channel->setPreviewurl(preview);
        channel->setGame(game);
        channel->setOnline(online);
        channel->setViewers(viewers);

        favouritesModel->addChannel(channel);

        emit addedChannel(channel->getId());

        Channel *chan = resultsModel->find(channel->getId());
        if (chan){
            chan->setFavourite(true);
            resultsModel->updateChannelForView(chan);
        }

        //Update featured also
        chan = featuredModel->find(channel->getId());
        if (chan){
            chan->setFavourite(true);
            featuredModel->updateChannelForView(chan);
        }

        //netman->getStream(channel->getServiceName());
    }
}

ChannelManager::ChannelManager(){
    alert = true;

    alertPosition = 1;

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
    if (!QFile::exists(appPath())){
        QFile file(appPath());
        file.open(QIODevice::ReadWrite);
        file.write("{}");
    }
}



bool ChannelManager::load(){

    qDebug() << "Opening JSON...";

    QJsonParseError error;
    if (!QFile::exists(appPath())){
        qDebug() << "File doesn't exist";
        return false;
    }
    QJsonObject json = QJsonDocument::fromJson(util::readFile(appPath()).toUtf8(),&error).object();

    if (error.error != QJsonParseError::NoError){
        qDebug() << "Parsing error!";
        return false;
    }

    if (!json["alert"].isNull()){
        alert = json["alert"].toBool();
    }

    if (!json["alertPosition"].isNull()){
        alertPosition = json["alertPosition"].toInt();
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

    QList<Channel*> _channels;

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

        _channels.append(channel);
    }

    favouritesModel->addAll(_channels);

    qDeleteAll(_channels);

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

    obj["alertPosition"] = QJsonValue(alertPosition);

    return util::writeFile(appPath(),QJsonDocument(obj).toJson());
}

void ChannelManager::addToFavourites(const quint32 &id){
    Channel *channel = resultsModel->find(id);

    if (!channel){
        channel = featuredModel->find(id);
    }

    if (channel){
        favouritesModel->addChannel(new Channel(*channel));

        channel->setFavourite(true);
        emit addedChannel(channel->getId());

        resultsModel->updateChannelForView(channel);

        //Update featured also
        featuredModel->updateChannelForView(channel);

        save();
    }
}

Channel* ChannelManager::findFavourite(const QString &q){
    return favouritesModel->find(q);
}

void ChannelManager::removeFromFavourites(const quint32 &id){
    Channel *chan = favouritesModel->find(id);

    emit deletedChannel(chan->getId());

    favouritesModel->removeChannel(chan);

    chan = 0;

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

    save();
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

        if (!channel->isOnline())
            needsStreamCheck = true;
    }

    resultsModel->addAll(list);

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

void ChannelManager::setAlert(const bool &val)
{
    alert = val;
}

void ChannelManager::addFeaturedResults(const QList<Channel *> &list)
{
    foreach (Channel *channel, list){
        if (favouritesModel->find(channel->getId()))
            channel->setFavourite(true);
    }

    featuredModel->addAll(list);

    qDeleteAll(list);

    emit featuredUpdated();
}

void ChannelManager::updateFavourites(const QList<Channel*> &list)
{
    favouritesModel->updateChannels(list);
    qDeleteAll(list);
}

bool ChannelManager::containsFavourite(const quint32 &q)
{
    return favouritesModel->find(q) > 0;
}

//Updates channel streams in all models
void ChannelManager::updateStreams(const QList<Channel*> &list)
{
    favouritesModel->updateStreams(list);
    featuredModel->updateStreams(list);
    resultsModel->updateStreams(list);
    qDeleteAll(list);
}

void ChannelManager::addGames(const QList<Game*> &list)
{
    gamesModel->addAll(list);

    qDeleteAll(list);

    emit gamesUpdated();
}

void ChannelManager::notify(Channel *channel)
{
    if (alert && channel){
        /*QStringList args;
        args << channel->getName() + (channel->isOnline() ? " is now streaming" : " has gone offline");
        args << channel->getInfo() << channel->getLogourl();

        emit pushNotification(args);*/

        emit pushNotification(channel->getName() + (channel->isOnline() ? " is now streaming" : " has gone offline"),
                              channel->getInfo(),
                              channel->getLogourl());
    }
}
