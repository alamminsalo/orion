#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "channellistmodel.h"
#include "game.h"
#include "../util/fileutils.h"
#include "../network/networkmanager.h"
#include <QStringRef>
#include <QObject>
#include <QDir>
#include <QProcess>
#include <QSortFilterProxyModel>

#define DATA_FILE           "data.json"
#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"
#define DIALOG_FILE         "resources/scripts/dialog.sh"
#define PLAY_FILE           "resources/scripts/play.sh"

class NetworkManager;

class ChannelManager: public QObject{
    Q_OBJECT
    //Q_PROPERTY(QVariantList channels READ getChannelsList NOTIFY channelsUpdated)
    //Q_PROPERTY(QVariantList results READ getResultsList NOTIFY resultsUpdated)
    //Q_PROPERTY(QVariantList games READ getGamesList NOTIFY gamesUpdated)

protected:
    NetworkManager* netman;

    ChannelListModel* favouritesModel;
    QSortFilterProxyModel* favouritesProxy;

    ChannelListModel* resultsModel;
    QSortFilterProxyModel* resultsProxy;

    QList<Game*> games;

public:
    ChannelManager();
    ~ChannelManager();

    bool load();
    bool save() const;
    bool writeJSON(const QString&);

    //void checkChannels();

    //Favourites section
    Channel *find(const QString&);
    void addToFavourites(Channel *channel);
    void removeFromFavourites(const QString&);
    void updateFavourites(const QList<Channel*>&);

    //Search section
    void addSearchResults(const QList<Channel*>&);

    void play(const QString&);
    void checkResources();

    void checkStreams(const QList<Channel*>&);

    void updateStreams(const QList<Channel*>&);
    void updateGames(const QList<Game*>&);

    ChannelListModel *getFavouritesModel() const;

    QSortFilterProxyModel *getFavouritesProxy() const;

    ChannelListModel *getResultsModel() const;

    QSortFilterProxyModel *getResultsProxy() const;

signals:
    void channelExists(Channel*);
    void channelNotFound(Channel*);
    void channelStateChanged(Channel*);
    void newChannel(Channel*);
    void gamesUpdated();
    void channelsUpdated();
    void resultsUpdated();

public slots:
    void pollFavourites();
    void getGames();
    void searchChannels(const QString&, const quint32&, const quint32&, bool);
    void notify(Channel*);

};

#endif //CHANNEL_MANAGER_H
