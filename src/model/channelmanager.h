#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "channellistmodel.h"
#include "gamelistmodel.h"
#include "game.h"
#include "../network/networkmanager.h"
#include "../notifications/notification.h"

#include <QSortFilterProxyModel>

#define DATA_FILE           "data.json"
#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"
#define DIALOG_FILE         "resources/scripts/dialog.sh"
#define PLAY_FILE           "resources/scripts/play.sh"

class NetworkManager;

class ChannelManager: public QObject{
    Q_OBJECT

protected:
    NetworkManager* netman;

    ChannelListModel* favouritesModel;
    QSortFilterProxyModel* favouritesProxy;

    ChannelListModel* resultsModel;

    ChannelListModel* featuredModel;
    QSortFilterProxyModel* featuredProxy;

    GameListModel* gamesModel;

    SnoreNotif notif;

    bool alert;
    quint16 cache;

public:
    ChannelManager();
    ~ChannelManager();

    bool load();
    bool save() const;
    bool writeJSON(const QString&);

    Channel *find(const QString&);
    void updateFavourites(const QList<Channel*>&);

    //Search section
    void addSearchResults(const QList<Channel*>&);

    void addFeaturedResults(const QList<Channel*>&);

    void play(const QString&);
    void checkResources();

    void checkStreams(const QList<Channel*>&);

    void updateStreams(const QList<Channel*>&);
    void updateGames(const QList<Game*>&);

    ChannelListModel *getFavouritesModel() const;

    QSortFilterProxyModel *getFavouritesProxy() const;

    ChannelListModel *getResultsModel() const;

    GameListModel *getGamesModel() const;

    QSortFilterProxyModel *getFeaturedProxy() const;

    Q_INVOKABLE quint16 getCache() const;
    Q_INVOKABLE bool isAlert() const;

signals:
    void channelExists(Channel*);
    void channelNotFound(Channel*);
    void channelStateChanged(Channel*);
    void newChannel(Channel*);
    void gamesUpdated();
    void channelsUpdated();
    void resultsUpdated();
    void featuredUpdated();
    void searchingStarted();
    void foundPlaybackStream(const QString &stream);
    void cacheUpdated();

public slots:
    void checkFavourites();
    void addToFavourites(const quint32&);
    void removeFromFavourites(const quint32&);
    void getGames(const quint32&, const quint32&, bool);
    void searchChannels(QString, const quint32&, const quint32&, bool);
    void notify(Channel*);
    void getFeatured();
    void findPlaybackStream(const QString&, const qint32&);
    void setCache(const quint16&);
    void setAlert(const bool&);
};

#endif //CHANNEL_MANAGER_H
