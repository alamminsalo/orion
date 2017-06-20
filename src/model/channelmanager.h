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

#pragma once

#include "channel.h"
#include "channellistmodel.h"
#include "gamelistmodel.h"
#include "game.h"
#include "../network/networkmanager.h"

#include <QSettings>
#include <QSortFilterProxyModel>
#include <QQmlEngine>
#include <QJSEngine>

#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

class ChannelManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool swapChat READ getSwapChat WRITE setSwapChat NOTIFY swapChatChanged)
    Q_PROPERTY(double textScaleFactor READ getTextScaleFactor WRITE setTextScaleFactor NOTIFY textScaleFactorChanged)
    Q_PROPERTY(bool offlineNotifications READ getOfflineNotifications WRITE setOfflineNotifications NOTIFY notificationsChanged)

    static ChannelManager *instance;

protected:
    NetworkManager* netman;

    ChannelListModel* tempFavourites;

    ChannelListModel* favouritesModel;
    QSortFilterProxyModel* favouritesProxy;

    ChannelListModel* resultsModel;

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
    double _textScaleFactor;
    QString quality;

    //Oauth
    QString user_name;
    quint64 user_id;
    QString access_token;

    /**
     * @brief createFollowedChannelsModel
     * Creates ready-to-use followed channels model
     * @return
     */
    ChannelListModel *createFollowedChannelsModel();

    ChannelManager();
public:
    static ChannelManager *getInstance();

    static QObject *provider(QQmlEngine */*eng*/, QJSEngine */*jseng*/) {
        QQmlEngine::setObjectOwnership(getInstance(), QQmlEngine::CppOwnership);
        return getInstance();
    }

    ~ChannelManager();

    void load();
    void save();

    Q_INVOKABLE bool containsFavourite(const quint32&);

    void checkStreams(const QList<Channel*>&);

    ChannelListModel *getFavouritesModel() const;

    QSortFilterProxyModel *getFavouritesProxy() const;

    ChannelListModel *getResultsModel() const;

    GameListModel *getGamesModel() const;

    Q_INVOKABLE bool isAlert() const;

    Q_INVOKABLE int getAlertPosition() const;
    Q_INVOKABLE void setAlertPosition(const int &value);

    Q_INVOKABLE QString getQuality() const;
    Q_INVOKABLE void setQuality(const QString & quality);

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

    void setSwapChat(bool value);
    bool getSwapChat();
    void setTextScaleFactor(double value);
    double getTextScaleFactor();

    void setOfflineNotifications(bool value);
    bool getOfflineNotifications();

    quint64 getUser_id() const;

signals:
    void pushNotification(const QString &title, const QString &message, const QString &imgUrl);
    void resultsUpdated(int numAdded, int total);
    void searchingStarted();
    void foundPlaybackStream(const QVariantMap &streams);
    void deletedChannel(const quint32 &chanid);
    void addedChannel(const quint32 &chanid);
    void gamesSearchStarted();
    void gamesUpdated();
    void followedUpdated();
    void swapChatChanged();
    void textScaleFactorChanged();
    void notificationsChanged();

    //oauth methods
    void accessTokenUpdated();
    void userNameUpdated(const QString name);
    void login(const QString &username, const QString &password);

public slots:
    void checkFavourites();
    void addToFavourites(const quint32&);
    void removeFromFavourites(const quint32&);
    void searchChannels(QString, const quint32&, const quint32&, bool);

    void notify(Channel*);
    void notifyMultipleChannelsOnline(const QList<Channel*> &);

    void findPlaybackStream(const QString&);
    void setAlert(const bool&);
    void setAccessToken(const QString &arg);

private slots:
    void addSearchResults(const QList<Channel*>&, const int total);
    void updateFavourites(const QList<Channel*>&);
    void updateStreams(const QList<Channel*>&);
    void addGames(const QList<Game*>&);
    void onUserUpdated(const QString &name, const quint64 userId);
    void addFollowedResults(const QList<Channel*>&, const quint32, const quint32);
    void onNetworkAccessChanged(bool);
};
