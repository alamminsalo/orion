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

#include <QSortFilterProxyModel>

#define DATA_FILE           "data.json"
#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"
#define DIALOG_FILE         "resources/scripts/dialog.sh"
#define PLAY_FILE           "resources/scripts/play.sh"

class NetworkManager;

QString appPath();

class ChannelManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString username READ username NOTIFY userNameUpdated)
    Q_PROPERTY (QString accesstoken READ accessToken NOTIFY accessTokenUpdated)

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

    bool alert;
    bool closeToTray;
    int alertPosition;

    //Oauth
    QString user_name;
    QString access_token;

public:
    ChannelManager(NetworkManager *netman);
    ~ChannelManager();

    bool load();
    bool save();
    bool writeJSON(const QString&);

    Channel *findFavourite(const QString&);
    Q_INVOKABLE bool containsFavourite(const quint32&);

    void play(const QString&);
    void checkResources();

    void checkStreams(const QList<Channel*>&);

    ChannelListModel *getFavouritesModel() const;

    QSortFilterProxyModel *getFavouritesProxy() const;

    ChannelListModel *getResultsModel() const;

    GameListModel *getGamesModel() const;

    QSortFilterProxyModel *getFeaturedProxy() const;

    Q_INVOKABLE bool isAlert() const;

    Q_INVOKABLE int getAlertPosition() const;
    Q_INVOKABLE void setAlertPosition(const int &value);

    Q_INVOKABLE void addToFavourites(const quint32 &id, const QString &serviceName, const QString &title,
                                     const  QString &info, const QString &logo, const QString &preview,
                                     const QString& game, const qint32 &viewers, bool online);
    Q_INVOKABLE bool isCloseToTray() const;
    Q_INVOKABLE void setCloseToTray(bool arg);
    Q_INVOKABLE void getFollowedChannels(const quint32 &limit = FOLLOWED_FETCH_LIMIT, const quint32 &offset = 0);
    Q_INVOKABLE void searchGames(QString, const quint32&, const quint32&);

    Q_INVOKABLE QString username() const;
    Q_INVOKABLE QString accessToken() const;

    Q_INVOKABLE void setAccessToken(const QString &arg);
    Q_INVOKABLE bool isAccessTokenAvailable() { return access_token.length() > 0; }

signals:
    void pushNotification(const QString &title, const QString &message, const QString &imgUrl);
    void resultsUpdated();
    void featuredUpdated();
    void searchingStarted();
    void foundPlaybackStream(const QStringList streams);
    void deletedChannel(const quint32 &chanid);
    void addedChannel(const quint32 &chanid);
    void gamesSearchStarted();
    void gamesUpdated();
    void followedUpdated();

    //oauth methods
    void accessTokenUpdated();
    void userNameUpdated(const QString name);
    void login(QString username, QString password);

public slots:
    void checkFavourites();
    void addToFavourites(const quint32&);
    void removeFromFavourites(const quint32&);
    void searchChannels(QString, const quint32&, const quint32&, bool);

    void notify(Channel*);
    void getFeatured();
    void findPlaybackStream(const QString&);
    void setAlert(const bool&);
    void onFoundPlaybackStream(const QStringList &);

private slots:
    void addSearchResults(const QList<Channel*>&);
    void addFeaturedResults(const QList<Channel*>&);
    void updateFavourites(const QList<Channel*>&);
    void updateStreams(const QList<Channel*>&);
    void addGames(const QList<Game*>&);
    void onUserNameUpdated(const QString &name);
    void addFollowedResults(const QList<Channel*>&, const quint32);
    void onNetworkAccessChanged(bool);
};

#endif //CHANNEL_MANAGER_H
