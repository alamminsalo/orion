/*
 * Copyright © 2015-2016 Andrew Penkrat
 *
 * This file is part of TwitchTube.
 *
 * TwitchTube is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TwitchTube is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TwitchTube.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IRCCHAT_H
#define IRCCHAT_H

#include <QObject>
#include <QTcpSocket>
#include <QNetworkReply>
#include <QMap>
#include <QRegExp>
#include <QColor>
#include <QQmlListProperty>
#include <QImage>
#include <QFile>
#include <QHash>
#include <QDir>
#include <QQuickImageProvider>
//#include "messagelistmodel.h"
//#include "message.h"
#include "imageprovider.h"
#include "channelmanager.h"

//#define TWITCH_EMOTE_URI "https://static-cdn.jtvnw.net/emoticons/v1/%d/1.0"

struct ChatMessage {
    QString name;
    QVariantList messageList;
    QString color;
    bool subscriber;
    bool turbo;
    bool mod;
    bool isAction;
    QVariantList badges;
    bool isChannelNotice;
    QString systemMessage;
};

// Backend for chat
class IrcChat : public QObject
{
    Q_OBJECT
public:
    IrcChat(QObject *parent = 0);
    ~IrcChat();

    Q_PROPERTY(QString name MEMBER username)
    Q_PROPERTY(QString password MEMBER userpass)
    Q_PROPERTY(bool anonymous READ anonymous WRITE setAnonymous NOTIFY anonymousChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool inRoom READ inRoom)
    Q_PROPERTY(QString emoteDirPath MEMBER emoteDirPathImpl)
    Q_PROPERTY(QList<int> emoteSetIDs READ emoteSetIDs NOTIFY emoteSetIDsChanged)

    Q_INVOKABLE void join(const QString channel, const QString channelId);
    Q_INVOKABLE void replay(const QString channel, const QString channelId, const quint64 vodId, double vodStartEpochTime, double playbackOffset);
    Q_INVOKABLE void replaySeek(double newOffset);
    Q_INVOKABLE void replayUpdate(double newOffset);
    Q_INVOKABLE void replayStop();
    Q_INVOKABLE void leave();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void reopenSocket();
    Q_INVOKABLE void initProviders();
    Q_INVOKABLE void hookupChannelProviders(ChannelManager * cman);
    Q_INVOKABLE QString getBadgeLocalUrl(QString key);

    //# User
    QString username, userpass;
    inline bool anonymous() { return anonym; }
    void setAnonymous(bool newAnonymous);
    bool anonym;
    QStringList userSpecs;
    QString userGlobalDisplayName;
    QMap<QString, QString> userChannelDisplayName;

    //# Network
    bool connected();
    inline bool inRoom() { return !room.isEmpty(); }

    //emote download
    QDir emoteDir;
    QString emoteDirPathImpl;
    QList<int> _emoteSetIDs;
    QList<int> emoteSetIDs();

    void RegisterEngineProviders(QQmlEngine & engine);

signals:
    void errorOccured(QString errorDescription);    
    void connectedChanged();
    void emoteSetIDsChanged();
    void anonymousChanged();
    void messageReceived(QString user, QVariantList message, QString chatColor, bool subscriber, bool turbo, bool mod, bool isAction, QVariantList badges, bool isChannelNotice, QString systemMessage);
    void noticeReceived(QString message);
    void myBadgesForChannel(QString channel, QList<QPair<QString, QString>> badges);

    void downloadComplete();
    bool downloadError();
    
public slots:
    void sendMessage(const QString &msg, const QVariantMap &relevantEmotes);
    void onSockStateChanged();
    void login();

    bool bulkDownloadEmotes(QList<QString> keys);

private slots:
    void createConnection();
    void receive();
    void processError(QAbstractSocket::SocketError socketError);
    void handleDownloadComplete();
    void handleVodStartTime(double);
    void handleDownloadedReplayChat(QList<ReplayChatMessage>);

private:
    static const qint16 PORT;
    static const QString HOST;

    static const QString IMAGE_PROVIDER_EMOTE;
    static const QString EMOTICONS_URL_FORMAT;

    URLFormatImageProvider _emoteProvider;
    BadgeImageProvider * _badgeProvider;
    ChannelManager * _cman;
    
    QList<ChatMessage> msgQueue;

    void parseCommand(QString cmd);

    struct CommandParse {
        QString channel;
        bool wrongChannel;
        ChatMessage chatMessage;
        QString params;
        bool haveMessage;
        QString message;
        QList<QString> tags;
        QString emotesStr;
    };

    void parseMessageCommand(const QString cmd, const QString cmdKeyword, CommandParse & commandParse);
    QMap<int, QPair<int, int>> parseEmotesTag(const QString emotes);
    void createEmoteMessageList(const QMap<int, QPair<int, int>> & emotePositionsMap, QVariantList & messageList, const QString message);
    void addWordSplit(const QString & s, const QChar & sep, QVariantList & l);
    QString getParamValue(QString params, QString param);
    QTcpSocket *sock;
    QString room;
    QString roomChannelId;
    // map of channel name -> list of pairs (badge name, badge version)
    QMap<QString, QList<QPair<QString, QString>>> badgesByChannel;
    bool logged_in;
    void disposeOfMessage(ChatMessage m);
    QVariantList substituteEmotesInMessage(const QVariantList & message, const QVariantMap &relevantEmotes);
    bool addBadges(QVariantList & badges, const QString channel);
    void makeBadgeAvailable(const QString badgeName, const QString version);
    QString userGlobalColor;
    QMap<QString, QString> userChannelColors;
    QMap<QString, bool> userChannelMod;
    QMap<QString, bool> userChannelSubscriber;
    bool allDownloadsComplete();

    bool replayChatRequestInProgress;
    
    double replayChatVodStartTime;
    double replayChatFirstChunkTime;
    double replayChatCurrentSeekOffset;
    double replayChatCurrentTime; // the position that playback is currently at in chat
    double nextChatChunkTimestamp;
    
    quint64 replayVodId;
    QList<ReplayChatMessage> replayChatMessagesPending;

    void replayChatMessage(const ReplayChatMessage &);
    void replayUpdateCommon();
};

#endif // IRCCHAT_H
