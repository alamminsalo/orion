#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "../model/channelmanager.h"

#define KRAKEN_API "https://api.twitch.tv/kraken"
#define TWITCH_API "https://api.twitch.tv/api"

class ChannelManager;

class NetworkManager: public QObject
{
    Q_OBJECT

protected:
    QNetworkAccessManager* channelOperation;
    QNetworkAccessManager* streamOperation;
    QNetworkAccessManager* allStreamsOperation;
    QNetworkAccessManager* logoOperation;
    QNetworkAccessManager* genericFileOperation;
    QNetworkAccessManager* searchOperation;
    QNetworkAccessManager* gamesOperation;
    QNetworkAccessManager* gameStreamsOperation;
    QNetworkAccessManager* featuredStreamsOperation;
    QNetworkAccessManager* extractChannelStreamsOperation;
    QNetworkAccessManager* m3u8StreamsOperation;

    ChannelManager* cman;

public:
    NetworkManager(ChannelManager*);
    ~NetworkManager();

    void getChannel(const QString&);
    void getStream(const QString&);
    void getStreams(const QString&);
    void getLogo(Channel*);
    void getFile(const QString&, const QString&);
    void getGames(const quint32&, const quint32&);
    void searchChannels(const QString&, const quint32&, const quint32&);
    void getFeaturedStreams();
    void getStreamsForGame(const QString&, const quint32&, const quint32&);
    void getChannelPlaybackStream(const QString&, const quint32&);
    void getChannelm3u8(const QString&, const quint32&);

public slots:
    void channelReply(QNetworkReply*);
    void streamReply(QNetworkReply*);
    void allStreamsReply(QNetworkReply*);
    void logoReply(QNetworkReply*);
    void fileReply(QNetworkReply*);
    void gamesReply(QNetworkReply*);
    void gameStreamsReply(QNetworkReply*);
    void featuredStreamsReply(QNetworkReply*);
    void searchChannelsReply(QNetworkReply*);
    void channelPlaybackStreamReply(QNetworkReply*);
    void m3u8Reply(QNetworkReply*);
};

#endif // NETWORKMANAGER_H
