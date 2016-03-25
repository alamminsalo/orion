#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkInterface>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include "urls.h"

#include "../model/channel.h"
#include "../model/game.h"
#include "../model/vod.h"

#define ONLY_BROADCASTS true
#define USE_HLS true

class NetworkManager: public QObject
{
    Q_OBJECT

protected:

    void selectNetworkInterface();
    void testConnection();

    enum M3U8TYPE {
        LIVE = QNetworkRequest::CustomVerbAttribute + 1,
        VOD
    };

    void getM3U8Data(const QString&, M3U8TYPE type);

public:
    NetworkManager();
    ~NetworkManager();

    void getStreams(const QString&);
    void getGames(const quint32&, const quint32&);
    void searchChannels(const QString&, const quint32&, const quint32&);
    void searchGames(const QString&);
    void getFeaturedStreams();
    void getStreamsForGame(const QString&, const quint32&, const quint32&);
    void getChannelPlaybackStream(const QString&);
    void getBroadcasts(const QString channelName, quint32 offset, quint32 limit);
    void getBroadcastPlaybackStream(const QString &vod);

    QNetworkAccessManager *getManager() const;

signals:
    void finishedConnectionTest();

    void allStreamsOperationFinished(const QList<Channel *>&);
    void gamesOperationFinished(const QList<Game *>&);
    void gameStreamsOperationFinished(const QList<Channel *>&);
    void featuredStreamsOperationFinished(const QList<Channel *>&);
    void searchChannelsOperationFinished(const QList<Channel *>&);
    void searchGamesOperationFinished(const QList<Game *>&);
    void broadcastsOperationFinished(const QList<Vod *>&);
    void m3u8OperationFinished(const QStringList&);
    void m3u8OperationBFinished(const QStringList&);
    void fileOperationFinished(const QByteArray&);

private slots:
    void testConnectionReply();
    void handleSslErrors(QNetworkReply * reply, QList<QSslError> errors);
    void allStreamsReply();
    void gamesReply();
    void gameStreamsReply();
    void featuredStreamsReply();
    void searchChannelsReply();
    void searchGamesReply();
    void streamExtractReply();
    void m3u8Reply();
    void broadcastsReply();

private:
    QNetworkAccessManager *operation;

    bool connectionOK;
};

#endif // NETWORKMANAGER_H
