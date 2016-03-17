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

protected:
    NetworkManager* netman;

    ChannelListModel* favouritesModel;
    QSortFilterProxyModel* favouritesProxy;

    ChannelListModel* resultsModel;

    ChannelListModel* featuredModel;
    QSortFilterProxyModel* featuredProxy;

    GameListModel* gamesModel;

    bool alert;
    int alertPosition;

public:
    ChannelManager(NetworkManager *netman);
    ~ChannelManager();

    bool load();
    bool save() const;
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

signals:
    void channelExists(Channel*);
    void channelNotFound(Channel*);
    void channelStateChanged(Channel*);
    void pushNotification(const QString &title, const QString &message, const QString &imgUrl);
    void newChannel(Channel*);
    void gamesUpdated();
    void channelsUpdated();
    void resultsUpdated();
    void featuredUpdated();
    void searchingStarted();
    void foundPlaybackStream(const QStringList &streams);
    void deletedChannel(const quint32 &chanid);
    void addedChannel(const quint32 &chanid);

public slots:
    void checkFavourites();
    void addToFavourites(const quint32&);
    void removeFromFavourites(const quint32&);
    void getGames(const quint32&, const quint32&, bool);
    void searchChannels(QString, const quint32&, const quint32&, bool);
    void notify(Channel*);
    void getFeatured();
    void findPlaybackStream(const QString&);
    void setAlert(const bool&);
    void onFoundPlaybackStream(const QStringList &);

    void addSearchResults(const QList<Channel*>&);
    void addFeaturedResults(const QList<Channel*>&);
    void updateFavourites(const QList<Channel*>&);
    void updateStreams(const QList<Channel*>&);
    void addGames(const QList<Game*>&);
};

#endif //CHANNEL_MANAGER_H
