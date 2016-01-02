#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "../util/jsonparser.h"
#include "../model/channelmanager.h"
#include "../util/fileutils.h"

#define TWITCH_URI "https://api.twitch.tv/kraken"

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
    void getStreamsForGame(const QString&, const quint32&, const quint32&);

public slots:
    void channelReply(QNetworkReply*);
    void streamReply(QNetworkReply*);
    void allStreamsReply(QNetworkReply*);
    void logoReply(QNetworkReply*);
    void fileReply(QNetworkReply*);
    void gamesReply(QNetworkReply*);
    void gameStreamsReply(QNetworkReply*);
    void searchChannelsReply(QNetworkReply*);
};

#endif // NETWORKMANAGER_H
