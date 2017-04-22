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

#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "channellistmodel.h"
#include "gamelistmodel.h"
#include "game.h"
#include "../network/networkmanager.h"
#include "imageprovider.h"

#include <QSettings>
#include <QSortFilterProxyModel>

#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

//class NetworkManager;

class ChannelManager;

class BadgeImageProvider : public ImageProvider {
public:
    BadgeImageProvider(ChannelManager * channelManager);
    void setChannelName(QString channelName) { _channelName = channelName; }
    void setChannelId(QString channelId) { _channelId = channelId; }
    virtual QString getCanonicalKey(QString key);
protected:
    virtual const QUrl getUrlForKey(QString & key);
private:
    ChannelManager * _channelManager;
    QString _channelName;
    QString _channelId;
};

class ChannelManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool swapChat READ getSwapChat WRITE setSwapChat NOTIFY swapChatChanged)
    Q_PROPERTY(bool offlineNotifications READ getOfflineNotifications WRITE setOfflineNotifications NOTIFY notificationsChanged)

protected:
    NetworkManager* netman;

    ChannelListModel* tempFavourites;

    ChannelListModel* favouritesModel;
    QSortFilterProxyModel* favouritesProxy;

    ChannelListModel* resultsModel;

    ChannelListModel* featuredModel;
    QSortFilterProxyModel* featuredProxy;

    //Games (and game search results)
    GameListModel* gamesModel;

    //TODO: move settings to own class
    bool alert;
    bool closeToTray;
    int alertPosition;
    int volumeLevel;
    bool minimizeOnStartup;
    bool _swapChat;
    bool offlineNotifications;

    //Oauth
    QString user_name;
    QString access_token;

    /**
     * @brief createFollowedChannelsModel
     * Creates ready-to-use followed channels model
     * @return
     */
    ChannelListModel *createFollowedChannelsModel();

    bool haveEmoteSets;
    QList<int> lastRequestedEmoteSetIDs;

    QMap<int, QMap<int, QString>> lastEmoteSets;
    QMap<QString, QMap<QString, QMap<QString, QString>>> channelBadgeUrls;
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, QString>>>> channelBadgeBetaUrls;

    BadgeImageProvider badgeImageProvider;

public:
    ChannelManager(NetworkManager *netman);
    ~ChannelManager();

    void load();
    void save();

    Channel *findFavourite(const QString&);
    Q_INVOKABLE bool containsFavourite(const quint32&);

    void checkStreams(const QList<Channel*>&);

    ChannelListModel *getFavouritesModel() const;

    QSortFilterProxyModel *getFavouritesProxy() const;

    ChannelListModel *getResultsModel() const;

    GameListModel *getGamesModel() const;

    QSortFilterProxyModel *getFeaturedProxy() const;

    Q_INVOKABLE bool isAlert() const;

    Q_INVOKABLE int getAlertPosition() const;
    Q_INVOKABLE void setAlertPosition(const int &value);

    Q_INVOKABLE int getVolumeLevel() const;
    Q_INVOKABLE void setVolumeLevel(const int &value);

    Q_INVOKABLE void addToFavourites(const quint32 &id, const QString &serviceName, const QString &title,
                                     const  QString &info, const QString &logo, const QString &preview,
                                     const QString& game, const qint32 &viewers, bool online);
    Q_INVOKABLE bool isCloseToTray() const;
    Q_INVOKABLE void setCloseToTray(bool arg);
    Q_INVOKABLE void getFollowedChannels(const quint32 &limit = FOLLOWED_FETCH_LIMIT, const quint32 &offset = 0);
    Q_INVOKABLE void searchGames(QString, const quint32&, const quint32&);

    Q_INVOKABLE QString username() const;
    Q_INVOKABLE QString accessToken() const;

    Q_INVOKABLE bool isAccessTokenAvailable() { return !access_token.isEmpty(); }

    Q_INVOKABLE bool isMinimizeOnStartup() const;
    Q_INVOKABLE void setMinimizeOnStartup(bool value);

    Q_INVOKABLE bool loadEmoteSets(bool reload, const QList<int> &emoteSetIDs);
    Q_INVOKABLE bool loadChannelBadgeUrls(const QString channel);
    Q_INVOKABLE bool loadChannelBetaBadgeUrls(int channel);

    Q_INVOKABLE void cancelLastVodChatRequest();
    Q_INVOKABLE void resetVodChat();
    Q_INVOKABLE void getVodStartTime(quint64 vodId);
    Q_INVOKABLE void getVodChatPiece(quint64 vodId, quint64 offset);

    void setSwapChat(bool value);
    bool getSwapChat();

    void setOfflineNotifications(bool value);
    bool getOfflineNotifications();
    
    BadgeImageProvider * getBadgeImageProvider() {
        return &badgeImageProvider;
    }

    bool getChannelBadgeUrl(const QString channel, const QString badgeName, const QString imageFormat, QString & outUrl) {
        auto channelEntry = channelBadgeUrls.find(channel);
        if (channelEntry != channelBadgeUrls.end()) {
            auto badgeEntry = channelEntry.value().find(badgeName);
            if (badgeEntry != channelEntry.value().end()) {
                auto urlEntry = badgeEntry.value().find(imageFormat);
                if (urlEntry != badgeEntry.value().end()) {
                    outUrl = urlEntry.value();
                    return true;
                }
            }
        }
        return false;
    }

    bool getChannelBadgeBetaUrl(const QString channel, const QString badgeName, const QString version, const QString imageFormat, QString & outUrl) {
        auto channelEntry = channelBadgeBetaUrls.find(channel);
        if (channelEntry != channelBadgeBetaUrls.end()) {
            auto badgeEntry = channelEntry.value().find(badgeName);
            if (badgeEntry != channelEntry.value().end()) {
                auto versionEntry = badgeEntry.value().find(version);
                if (versionEntry != badgeEntry.value().end()) {
                    auto urlEntry = versionEntry.value().find(imageFormat);
                    if (urlEntry != versionEntry.value().end()) {
                        outUrl = urlEntry.value();
                        return true;
                    }
                }
            }
        }
        return false;
    }

signals:
    void pushNotification(const QString &title, const QString &message, const QString &imgUrl);
    void resultsUpdated();
    void featuredUpdated();
    void searchingStarted();
    void foundPlaybackStream(const QVariantMap &streams);
    void deletedChannel(const quint32 &chanid);
    void addedChannel(const quint32 &chanid);
    void gamesSearchStarted();
    void gamesUpdated();
    void followedUpdated();
    void swapChatChanged();
    void notificationsChanged();

    //oauth methods
    void accessTokenUpdated();
    void userNameUpdated(const QString name);
    void login(const QString &username, const QString &password);
    void emoteSetsLoaded(QVariantMap emoteSets);
    void channelBadgeUrlsLoaded(const QString &channel, QVariantMap badgeUrls);
    void channelBadgeBetaUrlsLoaded(const QString &channel, QVariantMap badgeSetData);

    void vodStartGetOperationFinished(double);
    void vodChatPieceGetOperationFinished(QList<ReplayChatMessage>);

public slots:
    void checkFavourites();
    void addToFavourites(const quint32&);
    void removeFromFavourites(const quint32&);
    void searchChannels(QString, const quint32&, const quint32&, bool);

    void notify(Channel*);
    void notifyMultipleChannelsOnline(const QList<Channel*> &);

    void getFeatured();
    void findPlaybackStream(const QString&);
    void setAlert(const bool&);
    void setAccessToken(const QString &arg);

private slots:
    void addSearchResults(const QList<Channel*>&);
    void addFeaturedResults(const QList<Channel*>&);
    void updateFavourites(const QList<Channel*>&);
    void updateStreams(const QList<Channel*>&);
    void addGames(const QList<Game*>&);
    void onUserNameUpdated(const QString &name);
    void onEmoteSetsUpdated(const QMap<int, QMap<int, QString>>);
    void innerChannelBadgeUrlsLoaded(const QString, const QMap<QString, QMap<QString, QString>> badgeUrls);
    void innerChannelBadgeBetaUrlsLoaded(const int channelId, const QMap<QString, QMap<QString, QMap<QString, QString>>> badgeData);
    void innerGlobalBadgeBetaUrlsLoaded(const QMap<QString, QMap<QString, QMap<QString, QString>>> badgeData);
    void addFollowedResults(const QList<Channel*>&, const quint32);
    void onNetworkAccessChanged(bool);
};

#endif //CHANNEL_MANAGER_H
