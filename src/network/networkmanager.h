#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "../model/channel.h"
#include "../model/channelmanager.h"
#include "../util/fileutils.h"

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
    QNetworkAccessManager* gamesOperation;

    ChannelManager* cman;

public:
    NetworkManager(ChannelManager*);
    ~NetworkManager();

    void getChannelData(Channel*);
    void getStream(Channel*);
    void getAllStreams(const QString&);
    void getLogo(Channel*);
    void getFile(const QString&, const QString&);
    void getGames();

public slots:
    void channelReply(QNetworkReply*);
    void streamReply(QNetworkReply*);
    void allStreamsReply(QNetworkReply*);
    void logoReply(QNetworkReply*);
    void fileReply(QNetworkReply*);
    void gamesReply(QNetworkReply*);
};

#endif // NETWORKMANAGER_H
