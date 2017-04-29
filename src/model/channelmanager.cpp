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

#include "channelmanager.h"
#include <QStringRef>
#include <QDir>
#include <QProcess>
#include "../util/fileutils.h"
#include <QThread>
#include <QApplication>
#include <QStandardPaths>

BadgeImageProvider::BadgeImageProvider(ChannelManager * channelManager) : ImageProvider("badge", ".png"), _channelManager(channelManager) {
    
}

QString BadgeImageProvider::getCanonicalKey(QString key) {
    /** Resolve a key with just a badge name and version, specific to the current room, to a globally unique key for an official API or beta API badge */
    QString url;

    const QString betaImageFormat = "image_url_1x";
    const QString officialImageFormat = "image";

    int splitPos = key.indexOf("-");
    if (splitPos != -1) {
        const QString badge = key.left(splitPos);
        const QString version = key.mid(splitPos + 1);
        qDebug() << "badge hunt: channel name" << _channelName << "channel id" << _channelId << "badge" << badge << "version" << version;

        if (_channelManager->getChannelBadgeBetaUrl(_channelId, badge, version, betaImageFormat, url)) {
            return QList<QString>({ _channelId, badge, version, betaImageFormat }).join("-");
        }
        if (_channelManager->getChannelBadgeBetaUrl("GLOBAL", badge, version, betaImageFormat, url)) {
            return QList<QString>({ "GLOBAL", badge, version, betaImageFormat }).join("-");
        }
        if (_channelManager->getChannelBadgeUrl(_channelName, badge, officialImageFormat, url)) {
            return QList<QString>({ _channelName, badge, officialImageFormat }).join("-");
        }
        if (_channelManager->getChannelBadgeUrl("GLOBAL", badge, officialImageFormat, url)) {
            return QList<QString>({ "GLOBAL", badge, officialImageFormat }).join("-");
        }
    }

    qDebug() << "getCanonicalKey for badge" << key << "could not find a badge";
    return key;
}

const QUrl BadgeImageProvider::getUrlForKey(QString & key) {
    QString url;

    QList<QString> parts = key.split("-");
    if (parts.length() == 3) {
        if (_channelManager->getChannelBadgeUrl(parts[0], parts[1], parts[2], url)) {
            return url;
        }
    }
    else if (parts.length() == 4) {
        if (_channelManager->getChannelBadgeBetaUrl(parts[0], parts[1], parts[2], parts[3], url)) {
            return url;
        }
    }
    qDebug() << "Invalid badge cache key" << key;
    return QUrl();
}

ChannelListModel *ChannelManager::createFollowedChannelsModel()
{
    ChannelListModel *model = new ChannelListModel();

    connect(model, SIGNAL(channelOnlineStateChanged(Channel*)), this, SLOT(notify(Channel*)));
    connect(model, SIGNAL(multipleChannelsChangedOnline(const QList<Channel*> &)), this, SLOT(notifyMultipleChannelsOnline(const QList<Channel*> &)));

    return model;
}

ChannelManager::ChannelManager(NetworkManager *netman) : netman(netman), badgeImageProvider(this) {
    access_token = "";
    tempFavourites = 0;

    alert = true;
    closeToTray = false;
    alertPosition = 1;
    minimizeOnStartup = false;

    resultsModel = new ChannelListModel();

    featuredModel = new ChannelListModel();

    gamesModel = new GameListModel();

    //Setup followed channels model and it's signal chain
    favouritesModel = createFollowedChannelsModel();

    favouritesProxy = new QSortFilterProxyModel();

    favouritesProxy->setSortRole(ChannelListModel::Roles::ViewersRole);
    favouritesProxy->sort(0, Qt::DescendingOrder);
    favouritesProxy->setSourceModel(favouritesModel);
    featuredProxy = new QSortFilterProxyModel();
    featuredProxy->setSourceModel(featuredModel);
    featuredProxy->setSortRole(ChannelListModel::Roles::ViewersRole);
    featuredProxy->sort(0, Qt::DescendingOrder);

    connect(netman, SIGNAL(allStreamsOperationFinished(const QList<Channel*>&)), this, SLOT(updateStreams(QList<Channel*>)));
    connect(netman, SIGNAL(featuredStreamsOperationFinished(const QList<Channel*>&)), this, SLOT(addFeaturedResults(QList<Channel*>)));
    connect(netman, SIGNAL(gamesOperationFinished(const QList<Game*>&)), this, SLOT(addGames(QList<Game*>)));
    connect(netman, SIGNAL(gameStreamsOperationFinished(const QList<Channel*>&)), this, SLOT(addSearchResults(QList<Channel*>)));
    connect(netman, SIGNAL(searchChannelsOperationFinished(const QList<Channel*>&)), this, SLOT(addSearchResults(QList<Channel*>)));
    connect(netman, SIGNAL(m3u8OperationFinished(QVariantMap)), this, SIGNAL(foundPlaybackStream(QVariantMap)));
    connect(netman, SIGNAL(searchGamesOperationFinished(QList<Game*>)), this, SLOT(addGames(QList<Game*>)));

    connect(netman, SIGNAL(userNameOperationFinished(QString)), this, SLOT(onUserNameUpdated(QString)));
    connect(netman, SIGNAL(getEmoteSetsOperationFinished(const QMap<int, QMap<int, QString>>)), this, SLOT(onEmoteSetsUpdated(const QMap<int, QMap<int, QString>>)));
    connect(netman, SIGNAL(getChannelBadgeUrlsOperationFinished(const QString, const QMap<QString, QMap<QString, QString>>)), this, SLOT(innerChannelBadgeUrlsLoaded(const QString, const QMap<QString, QMap<QString, QString>>)));
    connect(netman, SIGNAL(getChannelBadgeBetaUrlsOperationFinished(const int, const QMap<QString, QMap<QString, QMap<QString, QString>>>)), this, SLOT(innerChannelBadgeBetaUrlsLoaded(const int, const QMap<QString, QMap<QString, QMap<QString, QString>>>)));
    connect(netman, SIGNAL(getGlobalBadgeBetaUrlsOperationFinished(const QMap<QString, QMap<QString, QMap<QString, QString>>>)), this, SLOT(innerGlobalBadgeBetaUrlsLoaded(const QMap<QString, QMap<QString, QMap<QString, QString>>>)));
    connect(netman, SIGNAL(favouritesReplyFinished(const QList<Channel*>&, const quint32)), this, SLOT(addFollowedResults(const QList<Channel*>&, const quint32)));
    connect(netman, SIGNAL(vodStartGetOperationFinished(double)), this, SIGNAL(vodStartGetOperationFinished(double)));
    connect(netman, SIGNAL(vodChatPieceGetOperationFinished(QList<ReplayChatMessage>)), this, SIGNAL(vodChatPieceGetOperationFinished(QList<ReplayChatMessage>)));
    connect(netman, SIGNAL(chatterListLoadOperationFinished(QMap<QString, QList<QString>>)), this, SLOT(processChatterList(QMap<QString, QList<QString>>)));

    connect(netman, SIGNAL(networkAccessChanged(bool)), this, SLOT(onNetworkAccessChanged(bool)));
    load();
}

ChannelManager::~ChannelManager(){
    qDebug() << "Destroyer: ChannelManager";

    save();

    delete favouritesModel;
    delete resultsModel;
    delete featuredModel;
    delete gamesModel;
    delete favouritesProxy;
    delete featuredProxy;
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

        if (isAccessTokenAvailable() && !user_name.isEmpty()) {
            netman->editUserFavourite(access_token, user_name, channel->getServiceName(), true);
        }

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

        if (!isAccessTokenAvailable())
            save();
    }
}

bool ChannelManager::isCloseToTray() const
{
    return closeToTray;
}

void ChannelManager::setCloseToTray(bool arg)
{
    closeToTray = arg;
}

void ChannelManager::searchGames(QString q, const quint32 &offset, const quint32 &limit)
{
    if (offset == 0 || !q.isEmpty())
        gamesModel->clear();

    //If query is empty, search games by viewercount
    if (q.isEmpty())
        netman->getGames(offset, limit);

    //Else by queryword
    else if (offset == 0)
        netman->searchGames(q);

    emit gamesSearchStarted();
}

QString ChannelManager::username() const
{
    return user_name;
}

QString ChannelManager::accessToken() const
{
    return access_token;
}

void ChannelManager::setAccessToken(const QString &arg)
{
    access_token = arg;

    if (isAccessTokenAvailable()) {
        //Fetch display name for logged in user
        netman->getUser(access_token);

        //move favs to tempfavs
        if (!tempFavourites) {
            tempFavourites = favouritesModel;

            favouritesModel = createFollowedChannelsModel();
            favouritesProxy->setSourceModel(favouritesModel);
        }
    }

    else {
        //Reload local favourites from memory
        if (tempFavourites) {
            delete favouritesModel;
            favouritesModel = tempFavourites;
            tempFavourites = 0;
            favouritesProxy->setSourceModel(favouritesModel);
        }

        emit login("", "");
    }

    emit accessTokenUpdated();
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

void ChannelManager::load(){
    QSettings settings("orion.application", "Orion");

    if (settings.contains("alert")) {
        alert = settings.value("alert").toBool();
    }

    if (settings.contains("alertPosition")) {
        alertPosition = settings.value("alertPosition").toInt();
    }

    if (settings.contains("closeToTray")) {
        closeToTray = settings.value("closeToTray").toBool();
    }

    if (settings.contains("minimizeOnStartup")) {
        minimizeOnStartup = settings.value("minimizeOnStartup").toBool();
    }

    if (settings.contains("channels")) {
        QList<Channel*> _channels;

        int size = settings.beginReadArray("channels");

        for(int i=0; i<size; i++){
            settings.setArrayIndex(i);
            _channels.append(new Channel(settings));
        }
        settings.endArray();

        favouritesModel->addAll(_channels);

        qDeleteAll(_channels);
    }

    if (settings.contains("access_token")) {
        setAccessToken(settings.value("access_token").toString());
    } else {
        setAccessToken("");
    }
    if (settings.contains("volumeLevel")) {
        setVolumeLevel(settings.value("volumeLevel").toInt());
    } else {
        setVolumeLevel(100);
    }
    if(!settings.value("swapChat").isNull()) {
        _swapChat = settings.value("swapChat").toBool();
    }

    if(!settings.value("notifications").isNull()) {
        offlineNotifications = settings.value("notifications").toBool();
    }
}

void ChannelManager::save()
{
    QSettings settings("orion.application", "Orion");

    if (!settings.isWritable())
        qDebug() << "Error: settings file not writable";

    if (tempFavourites) {
        delete favouritesModel;
        favouritesModel = tempFavourites;
        tempFavourites = 0;
    }

    settings.setValue("alert", alert);
    settings.setValue("alertPosition", alertPosition);
    settings.setValue("closeToTray", closeToTray);
    settings.setValue("access_token", access_token);
    settings.setValue("volumeLevel", volumeLevel);
    settings.setValue("minimizeOnStartup", minimizeOnStartup);
    settings.setValue("swapChat", _swapChat);
    settings.setValue("notifications", offlineNotifications);

    //Write channels
    settings.beginWriteArray("channels");
    for (int i=0; i < favouritesModel->count(); i++){
        settings.setArrayIndex(i);
        favouritesModel->getChannels().at(i)->writeToSettings(settings);
    }
    settings.endArray();
}

void ChannelManager::addToFavourites(const quint32 &id){
    Channel *channel = resultsModel->find(id);

    if (!channel){
        channel = featuredModel->find(id);
    }

    if (channel){

        if (isAccessTokenAvailable() && !user_name.isEmpty()) {
            netman->editUserFavourite(access_token, user_name, channel->getServiceName(), true);
        }

        favouritesModel->addChannel(new Channel(*channel));

        channel->setFavourite(true);
        emit addedChannel(channel->getId());

        resultsModel->updateChannelForView(channel);

        //Update featured also
        featuredModel->updateChannelForView(channel);

        if (!isAccessTokenAvailable())
            save();
    }
}

Channel* ChannelManager::findFavourite(const QString &q){
    return favouritesModel->find(q);
}

void ChannelManager::removeFromFavourites(const quint32 &id){
    Channel *chan = favouritesModel->find(id);

    emit deletedChannel(chan->getId());

    if (isAccessTokenAvailable() && !user_name.isEmpty()) {
        netman->editUserFavourite(access_token, user_name, chan->getServiceName(), false);
    }

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

    if (!isAccessTokenAvailable())
        save();
}

void ChannelManager::checkStreams(const QList<Channel *> &list)
{
    //Divide list to sublists for sanity
    int pos = 0;

    while(pos < list.length()) {

        //Take sublist, max 50 items
        QList<Channel*> sublist = list.mid(pos, 50);

        //Create comma-separated list of channels
        QString channelsUrl = "";
        foreach(Channel* channel, sublist){
            if (!channelsUrl.isEmpty())
                channelsUrl += ",";

            channelsUrl += channel->getServiceName();
        }

        //Fetch channels
        QString url = KRAKEN_API
                + QString("/streams?")
                + QString("limit=%1").arg(50) //Important!
                + QString("&channel=") + channelsUrl;
        netman->getStreams(url);

        //Shift pos by 50
        pos += sublist.length();
    }
}

void ChannelManager::checkFavourites()
{
    checkStreams(favouritesModel->getChannels());
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
    return favouritesModel->find(q) != nullptr;
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

        if (!channel->isOnline() && !offlineNotifications)
            //Skip offline notifications if set
            return;

        emit pushNotification(channel->getName() + (channel->isOnline() ? " is now streaming" : " has gone offline"),
                              channel->getInfo(),
                              channel->getLogourl());
    }
}

void ChannelManager::notifyMultipleChannelsOnline(const QList<Channel*> &channels)
{
    if (channels.size() == 1) {
        //Only one channel, send the usual notification
        notify(channels.at(0));
    }

    else if (alert) {
        //Send multi-notification
        QString str;

        foreach (Channel *c, channels) {

            //Omit channels after enough characters in message body
            if (str.size() > 80) {
                str.append("...");
                break;
            }

            str.append(!str.isEmpty() ? ", " : "");
            str.append(c->getName());
        }

        emit pushNotification("Channels are streaming", str, DEFAULT_LOGO_URL);
    }
}


//Login function
void ChannelManager::onUserNameUpdated(const QString &name)
{
    user_name = name;
    emit userNameUpdated(user_name);

    if (isAccessTokenAvailable()) {
        emit login(user_name, access_token);

        //Start using user followed channels
        getFollowedChannels(FOLLOWED_FETCH_LIMIT, 0);
    }
}

QVariantMap convertEmoteSets(const QMap<int, QMap<int, QString>> emoteSets) {
    QVariantMap out;
    for (auto setEntry = emoteSets.begin(); setEntry != emoteSets.end(); setEntry++) {
        QVariantMap cur;
        auto set = setEntry.value();
        for (auto emote = set.begin(); emote != set.end(); emote++) {
            cur.insert(QString::number(emote.key()), emote.value());
        }
        out.insert(QString::number(setEntry.key()), cur);
    }
    return out;
}

bool ChannelManager::loadEmoteSets(bool reload, const QList<int> &emoteSetIDs) {
    if (!haveEmoteSets || (emoteSetIDs != lastRequestedEmoteSetIDs)) {
        reload = true;
    }

    if (reload) {
        if (isAccessTokenAvailable()) {
            haveEmoteSets = false;
            lastRequestedEmoteSetIDs = emoteSetIDs;
            netman->getEmoteSets(access_token, emoteSetIDs);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        // ok to deliver cached emote sets
        emit emoteSetsLoaded(convertEmoteSets(lastEmoteSets));
        return true;
    }
}

QVariantMap convertBadges(const QMap<QString, QMap<QString, QString>> &badges) {
    QVariantMap out;
    for (auto x = badges.constBegin(); x != badges.constEnd(); x++) {
        QVariantMap cur;
        auto badgeEntries = x.value();
        for (auto y = badgeEntries.constBegin(); y != badgeEntries.constEnd(); y++) {
            cur.insert(y.key(), y.value());
        }
        out.insert(x.key(), cur);
    }
    return out;
}

QVariantMap convertBetaBadges(const QMap<QString, QMap<QString, QMap<QString, QString>>> &badges) {
    QVariantMap out;
    for (auto x = badges.constBegin(); x != badges.constEnd(); x++) {
        out.insert(x.key(), convertBadges(x.value()));
    }
    return out;
}

bool ChannelManager::loadChannelBadgeUrls(const QString channel) {
    auto result = channelBadgeUrls.find(channel);
    if (result != channelBadgeUrls.end()) {
        // deliver cached channel badge URLs
        emit channelBadgeUrlsLoaded(channel, convertBadges(result.value()));
        return false;
    }
    else {
        netman->getChannelBadgeUrls(access_token, channel);
        return true;
    }
}

bool ChannelManager::loadChannelBetaBadgeUrls(int channel) {
    bool out = false;

    const QString channelKey = QString::number(channel);
    auto result = channelBadgeBetaUrls.find(channelKey);
    if (result != channelBadgeBetaUrls.end()) {
        // deliver cached channel beta badge URLS
        emit channelBadgeBetaUrlsLoaded(channelKey, convertBetaBadges(result.value()));
    }
    else {
        netman->getChannelBadgeUrlsBeta(channel);
        out = true;
    }

    const QString GLOBAL_BADGES_IDENTIFIER = "GLOBAL";
    result = channelBadgeBetaUrls.find(GLOBAL_BADGES_IDENTIFIER);
    if (result != channelBadgeBetaUrls.end()) {
        // deliver cached channel beta badge URLS
        emit channelBadgeBetaUrlsLoaded(GLOBAL_BADGES_IDENTIFIER, convertBetaBadges(result.value()));
    }
    else {
        netman->getGlobalBadgesUrlsBeta();
        out = true;
    }

    return out;
}

void ChannelManager::loadChatterList(const QString channel) {
    netman->loadChatterList(channel);
}

void ChannelManager::getVodStartTime(quint64 vodId) {
    netman->getVodStartTime(vodId);
}

void ChannelManager::getVodChatPiece(quint64 vodId, quint64 offset) {
    netman->getVodChatPiece(vodId, offset);
}

void ChannelManager::cancelLastVodChatRequest() {
    netman->cancelLastVodChatRequest();
}

void ChannelManager::resetVodChat() {
    netman->resetVodChat();
}

void ChannelManager::onEmoteSetsUpdated(const QMap<int, QMap<int, QString>> updatedEmoteSets)
{
    lastEmoteSets = updatedEmoteSets;
    haveEmoteSets = true;

    //qDebug() << "emitting updated emote set" << updatedEmoteSets;

    emit emoteSetsLoaded(convertEmoteSets(updatedEmoteSets));
}

void ChannelManager::innerChannelBadgeUrlsLoaded(const QString channel, const QMap<QString, QMap<QString, QString>> badgeUrls)
{
    channelBadgeUrls.remove(channel);
    channelBadgeUrls.insert(channel, badgeUrls);

    emit channelBadgeUrlsLoaded(channel, convertBadges(badgeUrls));
}

void ChannelManager::innerChannelBadgeBetaUrlsLoaded(const int channelId, const QMap<QString, QMap<QString, QMap<QString, QString>>> badgeData)
{
    QString channelKey = QString::number(channelId);
    channelBadgeBetaUrls.remove(channelKey);
    channelBadgeBetaUrls.insert(channelKey, badgeData);

    emit channelBadgeBetaUrlsLoaded(channelKey, convertBetaBadges(badgeData));
}

void ChannelManager::innerGlobalBadgeBetaUrlsLoaded(const QMap<QString, QMap<QString, QMap<QString, QString>>> badgeData)
{
    const QString GLOBAL_BADGES_KEY = "GLOBAL";
    channelBadgeBetaUrls.remove(GLOBAL_BADGES_KEY);
    channelBadgeBetaUrls.insert(GLOBAL_BADGES_KEY, badgeData);

    emit channelBadgeBetaUrlsLoaded(GLOBAL_BADGES_KEY, convertBetaBadges(badgeData));
}

void ChannelManager::getFollowedChannels(const quint32& limit, const quint32& offset)
{
    //if (offset == 0)
    //favouritesModel->clear();

    netman->getUserFavourites(user_name, offset, limit);
}


void ChannelManager::addFollowedResults(const QList<Channel *> &list, const quint32 offset)
{
    //    qDebug() << "Merging channel data for " << list.size()
    //             << " items with " << offset << " offset.";

    favouritesModel->mergeAll(list);

    if (list.size() == FOLLOWED_FETCH_LIMIT)
        getFollowedChannels(FOLLOWED_FETCH_LIMIT, offset);

    checkStreams(list);

    qDeleteAll(list);

    emit followedUpdated();
}

void ChannelManager::processChatterList(QMap<QString, QList<QString>> chatters)
{
    QVariantMap out;
    for (auto groupEntry = chatters.constBegin(); groupEntry != chatters.constEnd(); groupEntry++) {
        QVariantList group;
        for (const auto & chatter : groupEntry.value()) {
            group.append(chatter);
        }
        out.insert(groupEntry.key(), group);
    }

    emit chatterListLoaded(out);
}

void ChannelManager::onNetworkAccessChanged(bool up)
{
    if (up) {
        if (isAccessTokenAvailable()) {
            //Relogin
            favouritesModel->clear();
            netman->getUser(access_token);
        }
    } else {
        qDebug() << "Network went down";
        favouritesModel->setAllChannelsOffline();
        resultsModel->setAllChannelsOffline();
        featuredModel->setAllChannelsOffline();
    }
}
int ChannelManager::getVolumeLevel() const {
    return volumeLevel;
}
void ChannelManager::setVolumeLevel(const int &value) {
    volumeLevel = value;
}

bool ChannelManager::isMinimizeOnStartup() const
{
    return minimizeOnStartup;
}

void ChannelManager::setMinimizeOnStartup(bool value)
{
    minimizeOnStartup = value;
}

void ChannelManager::setSwapChat(bool value) {
    _swapChat = value;
    emit swapChatChanged();
}

bool ChannelManager::getSwapChat() {
    return _swapChat;
}

void ChannelManager::setOfflineNotifications(bool value) {
    offlineNotifications = value;
    emit notificationsChanged();
}

bool ChannelManager::getOfflineNotifications() {
    return offlineNotifications;
}
