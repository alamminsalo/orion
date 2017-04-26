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

#include "ircchat.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>
#include <QImage>
#include <qqml.h>
#include <QtMath>
#include <QDateTime>

const QString IrcChat::IMAGE_PROVIDER_EMOTE = "emote";
const QString IrcChat::EMOTICONS_URL_FORMAT = "https://static-cdn.jtvnw.net/emoticons/v1/%1/1.0";

const qint16 IrcChat::PORT = 6667;
const QString IrcChat::HOST = "irc.twitch.tv";

IrcChat::IrcChat(QObject *parent) :
    QObject(parent),
    _emoteProvider(IMAGE_PROVIDER_EMOTE, EMOTICONS_URL_FORMAT, ".png", "emotes"),
    _badgeProvider(nullptr),
    _cman(nullptr),
    sock(nullptr)
    {

    logged_in = false;

    connect(&_emoteProvider, &ImageProvider::downloadComplete, this, &IrcChat::handleDownloadComplete);

    room = "";

	emoteDirPathImpl = _emoteProvider.getBaseUrl();
}

void IrcChat::initSocket() {
    // Open socket
    sock = new QTcpSocket(this);
    if(!sock) {
        emit errorOccured("Error creating socket");
    }
    else {
        connect(sock, SIGNAL(readyRead()), this, SLOT(receive()));
        connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));
        connect(sock, SIGNAL(connected()), this, SLOT(login()));
        connect(sock, SIGNAL(connected()), this, SLOT(onSockStateChanged()));
        connect(sock, SIGNAL(disconnected()), this, SLOT(onSockStateChanged()));
    }
}

void IrcChat::initProviders() {
    initSocket();
	auto engine = qmlEngine(this);
	RegisterEngineProviders(*engine);
}

void IrcChat::RegisterEngineProviders(QQmlEngine & engine) {
	engine.addImageProvider(IMAGE_PROVIDER_EMOTE, _emoteProvider.getQMLImageProvider());
    if (_badgeProvider) {
        engine.addImageProvider(_badgeProvider->getImageProviderName(), _badgeProvider->getQMLImageProvider());
    }
    else {
        qDebug() << "couldn't hook up badge provider as it was not available";
    }
}

void IrcChat::hookupChannelProviders(ChannelManager * cman) {
    if (cman) {
        _badgeProvider = cman->getBadgeImageProvider();
        connect(_badgeProvider, &ImageProvider::downloadComplete, this, &IrcChat::handleDownloadComplete);
        _cman = cman;
        connect(_cman, &ChannelManager::vodStartGetOperationFinished, this, &IrcChat::handleVodStartTime);
        connect(_cman, &ChannelManager::vodChatPieceGetOperationFinished, this, &IrcChat::handleDownloadedReplayChat);
    }
    else {
        qDebug() << "hookupChannelProviders got null";
    }
}

IrcChat::~IrcChat() {
    disconnect();
}

void IrcChat::join(const QString channel, const QString channelId) {

    if (inRoom())
        leave();

    // Save channel name for later use
    room = channel;
    roomChannelId = channelId;

    if (_badgeProvider) {
        _badgeProvider->setChannelName(channel);
        _badgeProvider->setChannelId(channelId);
    }

    if (!connected()) {
        reopenSocket();
    }

    // Join channel's chat room
    if (sock) {
        sock->write(("JOIN #" + channel + "\r\n").toStdString().c_str());
    }

    qDebug() << "Joined channel " << channel;
}

void IrcChat::replay(const QString channel, const QString channelId, const quint64 vodId, double vodStartEpochTime, double playbackOffset) {
    if (inRoom())
        leave();

    room = channel;
    roomChannelId = channelId;

    if (_badgeProvider) {
        _badgeProvider->setChannelName(channel);
        _badgeProvider->setChannelId(channelId);
    }

    replayVodId = vodId;

    replayChatVodStartTime = vodStartEpochTime;

    replayChatCurrentSeekOffset = playbackOffset;

    // Get start timestamp of the chat replay segments as the requests need to be aligned to chunk boundaries from this start
    _cman->getVodStartTime(vodId);
}

void IrcChat::handleVodStartTime(double startTime) {
    replayChatFirstChunkTime = startTime;
    replaySeek(replayChatCurrentSeekOffset);
}

double quantize(double value, double start, double multiple) {
    double rel = value - start;
    return start + qRound(qFloor(rel / multiple) * multiple);
}

const double CHAT_CHUNK_TIME = 30.0;

void IrcChat::replaySeek(double newOffset) {
    // we set a flag indicating that a request is in flight
    if (replayChatRequestInProgress) {
        _cman->cancelLastVodChatRequest();
    }
    _cman->resetVodChat();
    replayChatRequestInProgress = true;
    // we save the offset as the current time
    replayChatCurrentTime = replayChatVodStartTime + newOffset;
    qDebug() << "original vod playback start time" << QDateTime::fromMSecsSinceEpoch(replayChatCurrentTime * 1000.0, Qt::UTC);
    nextChatChunkTimestamp = quantize(replayChatCurrentTime, replayChatFirstChunkTime, CHAT_CHUNK_TIME);
    qDebug() << "quantized vod playback start time for chat chunk" << QDateTime::fromMSecsSinceEpoch(nextChatChunkTimestamp * 1000.0, Qt::UTC);
    // we dump any pending messages from other stuff
    replayChatMessagesPending.clear();
    // we'll do an initial request for starting offset chat right now.
    _cman->getVodChatPiece(replayVodId, nextChatChunkTimestamp);
    // time passes in the front end as the vod plays back.
}

void IrcChat::replayUpdate(double newOffset) {
    // every so often the front end will find that time has passed and update our time and do our timestamp update
    double newCurrentTime = replayChatVodStartTime + newOffset;
    replayChatCurrentTime = newCurrentTime;
    replayUpdateCommon();
}

QList<QPair<QString, QString>> parseBadges(const QString badgesStr);

void IrcChat::replayChatMessage(const ReplayChatMessage & replayMessage) {

    ChatMessage chatMessage;

    chatMessage.name = replayMessage.from;

    for (auto tag = replayMessage.tags.constBegin(); tag != replayMessage.tags.constEnd(); tag++) {
        if (tag.key() == "display-name") {
            QString displayName = tag.value().toString();
            if (displayName != "") {
                chatMessage.name = displayName;
            }
        }
        else if (tag.key() == "color") {
            chatMessage.color = tag.value().toString();
        }
        else if (tag.key() == "subscriber") {
            chatMessage.subscriber = tag.value().toBool();
        }
        else if (tag.key() == "turbo") {
            chatMessage.turbo = tag.value().toBool();
        }
        else if (tag.key() == "mod") {
            chatMessage.mod = tag.value().toBool();
        }
        else if (tag.key() == "badges") {
            QList<QPair<QString, QString>> badgesMap = parseBadges(tag.value().toString());
            for (auto entry = badgesMap.constBegin(); entry != badgesMap.constEnd(); entry++) {
                makeBadgeAvailable(entry->first, entry->second);
                chatMessage.badges.push_back(QVariantList({ entry->first, entry->second }));
            }
        }
        else if (tag.key() == "system-msg") {
            QString systemMessage = tag.value().toString();

            // \s -> space
            systemMessage.replace("\\s", " ");
            // double backslash -> single backslash
            systemMessage.replace("\\\\", "\\");

            chatMessage.systemMessage = systemMessage;
        }
    }

    if (replayMessage.command == "USERNOTICE") {
        chatMessage.isChannelNotice = true;
    }
    else {
        chatMessage.isChannelNotice = false;
    }

    QString message = replayMessage.message;

    // parse IRC action before applying emotes, as emote indices are relative to the content of the action
    const QString ACTION_PREFIX = QString(QChar(1)) + "ACTION ";
    const QString ACTION_SUFFIX = QString(QChar(1));
    if (message.startsWith(ACTION_PREFIX) && message.endsWith(ACTION_SUFFIX)) {
        chatMessage.isAction = true;
        message = message.mid(ACTION_PREFIX.length(), message.length() - ACTION_SUFFIX.length() - ACTION_PREFIX.length());
    }
    else {
        chatMessage.isAction = false;
    }

    for (int emoteId : replayMessage.emoteList) {
        _emoteProvider.makeAvailable(QString::number(emoteId));
    }
    
    QVariantList messageList;
    createEmoteMessageList(replayMessage.emotePositionsMap, messageList, message);

    //qDebug() << "messageList " << messageList;

    chatMessage.messageList = messageList;

    disposeOfMessage(chatMessage);
}

void IrcChat::replayUpdateCommon() {
    // time to start fetching chat in advance by
    const double CHAT_TIME_MARGIN = 5.0;

    double curVideoOffsetMS = (replayChatCurrentTime - replayChatVodStartTime) * 1000.0;

    // in a chat update check, we look at the current time emit whatever chat's timestamps are up
    while (!replayChatMessagesPending.empty() && (replayChatMessagesPending.first().videoOffset) <= curVideoOffsetMS) {
        auto message = replayChatMessagesPending.first();
        auto delay = (curVideoOffsetMS - (replayChatMessagesPending.first().videoOffset)) / 1000.0;

        if (delay > 1.0 || delay < -1.0) {
            qDebug() << "**********************************************************";
            qDebug() << "chat replay delay" << delay << "s -" << message.from << message.message;
            qDebug() << "**********************************************************";
        }

        replayChatMessage(message);
        replayChatMessagesPending.pop_front();
    }

    // there is some sketchy logic to figure out what timestamp we actually deem ourselves to have chat up until
    double nextChatTime = nextChatChunkTimestamp;
    // then if we're close enough to the end of the chat buffer and there isn't a request in progress, we request some more chat
    if (!replayChatRequestInProgress && replayChatCurrentTime > (nextChatTime - CHAT_TIME_MARGIN)) {
        replayChatRequestInProgress = true;
        _cman->getVodChatPiece(replayVodId, nextChatTime);
    }
        
}

void IrcChat::handleDownloadedReplayChat(QList<ReplayChatMessage> messages) {
    // eventually the initial chat response will arrive. we'll put the chat into a queue, and do a first chat update check

    replayChatMessagesPending.append(messages);

    qDebug() << "CHAT REPLAY PART; t=" << QDateTime::fromMSecsSinceEpoch(nextChatChunkTimestamp * 1000.0, Qt::UTC) << messages.length() << "records";
    
    nextChatChunkTimestamp += CHAT_CHUNK_TIME;

    replayChatRequestInProgress = false;

    replayUpdateCommon();
}

void IrcChat::replayStop() {
    if (replayChatRequestInProgress) {
        _cman->cancelLastVodChatRequest();
        replayChatRequestInProgress = false;
    }
    _cman->resetVodChat();
    replayChatMessagesPending.clear();
}

void IrcChat::leave()
{
    msgQueue.clear();
    if (sock) {
        sock->write(("PART #" + room + "\r\n").toStdString().c_str());
    }
    room = "";
}

void IrcChat::disconnect() {
    leave();
    if (sock) {
        sock->close();
    }
}

void IrcChat::reopenSocket() {
    if (sock) {
        qDebug() << "Reopening socket";
        if (sock->isOpen())
            sock->close();
        sock->open(QIODevice::ReadWrite);
        sock->connectToHost(HOST, PORT);
        if (!sock->isOpen()) {
            emit errorOccured("Error opening socket");
        }
    }
}

void IrcChat::setAnonymous(bool newAnonymous) {
    if(newAnonymous != anonym) {
        if(newAnonymous) {
            qsrand(QTime::currentTime().msec());
            username = "";
            username.sprintf("justinfan%06d", (qrand() % (1000000 - 100000)) + 100000);
            userpass = "blah";
        }
        anonym = newAnonymous;

        //login();

        emit anonymousChanged();
    }
}

bool IrcChat::connected() {
    if (sock) {
        return sock->state() == QTcpSocket::ConnectedState;
    }
}

QVariantMap createEmoteEntry(int emoteId, QString emoteText) {
    QVariantMap emoteObj;
    emoteObj.insert("emoteId", emoteId);
    emoteObj.insert("emoteText", emoteText);
    return emoteObj;
}

QVariantList IrcChat::substituteEmotesInMessage(const QVariantList & message, const QVariantMap &relevantEmotes) {
    QVariantList output;

    for (auto word = message.begin(); word != message.end(); word++) {
        bool spacePrefix = word != message.begin();
        QString emoteText = spacePrefix ? word->toString().mid(1) : word->toString();
        auto entry = relevantEmotes.find(emoteText);
        if (entry != relevantEmotes.end()) {
            int emoteId = entry.value().toInt();
            _emoteProvider.makeAvailable(QString::number(emoteId));
            if (spacePrefix) {
                output.append(" ");
            }
            output.append(createEmoteEntry(emoteId, emoteText));
        }
        else {
            output.append(*word);
        }
    }
    return output;
}

void removeVariantListPairByFirstValue(QVariantList list, const QVariant value) {
    for (auto it = list.begin(); it != list.end();) {

        if (!it->canConvert<QVariantList>() || it->toList().length() < 1) {
            ++it;
            continue;
        }

        if (it->toList()[0] == value) {
            it = list.erase(it);
        }
        else {
            ++it;
        }
    }
}

void IrcChat::makeBadgeAvailable(const QString badgeName, const QString version) {
    if (_badgeProvider) {
        _badgeProvider->makeAvailable(badgeName + "-" + version);
    }
    else {
        qDebug() << "can't make badge" << badgeName << version << "available because there is no _badgeProvider";
    }
}

QString IrcChat::getBadgeLocalUrl(QString key) {
    if (_badgeProvider) {
        return _badgeProvider->getBaseUrl() + "/" + _badgeProvider->getCanonicalKey(key);
    }
    else {
        qDebug() << "can't get badge url because there is no _badgeProvider";
        return "";
    }
}

bool IrcChat::addBadges(QVariantList &badges, QString channel) {
    qDebug() << "addBadges" << channel;
    auto channelEntry = badgesByChannel.find(channel);
    if (channelEntry != badgesByChannel.end()) {
        auto curBadges = channelEntry.value();
        for (auto badge = curBadges.constBegin(); badge != curBadges.constEnd(); badge++) {
            qDebug() << "badge" << channel << badge->first << badge->second;
            removeVariantListPairByFirstValue(badges, badge->first);
            const QString badgeName = badge->first;
            const QString badgeVersion = badge->second;
            makeBadgeAvailable(badgeName, badgeVersion);
            badges.push_back(QVariantList({ badgeName, badgeVersion }));
        }
        return true;
    }
    else {
        return false;
    }
}

void IrcChat::sendMessage(const QString &msg, const QVariantMap &relevantEmotes) {
    if (inRoom() && connected()) {
        if (sock) {
            sock->write(("PRIVMSG #" + room + " :" + msg + "\r\n").toStdString().c_str());
        }

		bool isAction = false;
		QVariantList message;
		const QString ME_PREFIX = "/me ";
		QString displayMessage = msg;
		if (displayMessage.startsWith(ME_PREFIX)) {
			isAction = true;
			displayMessage = displayMessage.mid(ME_PREFIX.length());
		}
		addWordSplit(displayMessage, ' ', message);
        message = substituteEmotesInMessage(message, relevantEmotes);

        const QString channelName = "#" + room;

        QVariantList userBadges;
        if (!addBadges(userBadges, channelName)) {
            addBadges(userBadges, "GLOBAL");
        }

        qDebug() << "Looking up color for #" << room;

        QString color = "";
        bool subscriber = false;
        bool turbo = false; // tag?
        bool mod = false;


        auto colorEntry = userChannelColors.find(channelName);
        if (colorEntry != userChannelColors.end()) {
            color = colorEntry.value();
            qDebug() << "using user room color" << color;
        }
        else {
            color = userGlobalColor;
            qDebug() << "using user global color" << color;
        }

        auto subscriberEntry = userChannelSubscriber.find(channelName);
        if (subscriberEntry != userChannelSubscriber.end()) {
            subscriber = subscriberEntry.value();
        }

        auto modEntry = userChannelMod.find(channelName);
        if (modEntry != userChannelMod.end()) {
            mod = modEntry.value();
        }

        QString displayName = username;
        
        auto displayNameEntry = userChannelDisplayName.find(channelName);
        if (displayNameEntry != userChannelDisplayName.end() && displayNameEntry.value() != "") {
            displayName = displayNameEntry.value();
        } else if (userGlobalDisplayName != "") {
            displayName = userGlobalDisplayName;
        }

        disposeOfMessage({ displayName, message, color, subscriber, turbo, mod, isAction, userBadges });
    }
}

void IrcChat::onSockStateChanged() {
    // We don't check if connected property actually changed because this slot should only be awaken when it did
    emit connectedChanged();
}

void IrcChat::login()
{
    if (userpass.isEmpty() || username.isEmpty())
        setAnonymous(true);
    else
        setAnonymous(false);

    if (sock) {
        // Tell server that we support twitch-specific commands
        sock->write("CAP REQ :twitch.tv/commands\r\n");
        sock->write("CAP REQ :twitch.tv/tags\r\n");

        // Login
        sock->write(("PASS " + userpass + "\r\n").toStdString().c_str());
        sock->write(("NICK " + username + "\r\n").toStdString().c_str());
    }

    logged_in = true;

    //Join room automatically, if given
    if (!room.isEmpty())
        join(room, roomChannelId);
}

void IrcChat::receive() {
    QString msg;
    while (sock && sock->canReadLine()) {
        msg = sock->readLine();
        msg = msg.remove('\n').remove('\r');
        parseCommand(msg);
    }
}

void IrcChat::processError(QAbstractSocket::SocketError socketError) {
    QString err;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        err = "Server closed connection.";
        break;
    case QAbstractSocket::HostNotFoundError:
        err = "Host not found.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        err = "Connection refused.";
        break;
    default:
        err = "Unknown error.";
    }

    emit errorOccured(err);
}

void IrcChat::addWordSplit(const QString & s, const QChar & sep, QVariantList & l) {
	bool first = true;
    const QStringList & parts = s.split(sep);
	for (const auto & part : parts) {
		if (first) {
			first = false;
			l.append(part);
		}
		else {
			l.append(QString(sep) + part);
		}
	}
}

bool IrcChat::allDownloadsComplete() {
    return !_emoteProvider.downloadsInProgress() && _badgeProvider && !_badgeProvider->downloadsInProgress();
}

void IrcChat::disposeOfMessage(ChatMessage m) {
    if (allDownloadsComplete()) {
        emit messageReceived(m.name, m.messageList, m.color, m.subscriber, m.turbo, m.mod, m.isAction, m.badges, m.isChannelNotice, m.systemMessage);
    }
    else {
        // queue message to be shown when downloads are complete
        msgQueue.push_back(m);
    }
}

QList<QString> getTags(const QString cmd) {
    if (cmd.at(0) == QChar('@')) {
        // tags are present
        int tagsEnd = cmd.indexOf(" ");
        QString tags = cmd.mid(1, tagsEnd - 1);
        return tags.split(";");
    }
    else {
        return QList<QString>();
    }
}

class Tag {
public:
    Tag(const QString tag) {
        int assignPos = tag.indexOf("=");
        if (assignPos == -1) {
            valid = false;
        }
        else {
            valid = true;
            key = tag.left(assignPos);
            value = tag.mid(assignPos + 1);
        }
    }
public:
    QString key;
    QString value;
    bool valid;
};

QList<QPair<QString, QString>> parseBadges(const QString badgesStr) {
    QList<QPair<QString, QString>> badges;
    foreach(const QString & badgeStr, badgesStr.split(",")) {
        int splitPos = badgeStr.indexOf('/');
        if (splitPos == -1) continue;
        badges.append(QPair<QString, QString>(badgeStr.left(splitPos), badgeStr.mid(splitPos + 1)));
    }
    return badges;
}

QMap<int, QPair<int, int>> IrcChat::parseEmotesTag(const QString emotes) {
    QMap<int, QPair<int, int>> emotePositionsMap;
    if (emotes != "") {
        auto emoteList = emotes.split('/');

        for (auto emote : emoteList) {
            auto key = emote.left(emote.indexOf(':'));
            auto positions = emote.remove(0, emote.indexOf(':') + 1);
            //qDebug() << "key " << key;

            _emoteProvider.makeAvailable(key);

            const QStringList & emotePlcs = positions.split(',');
            for (const auto & emotePlc : emotePlcs) {
                auto firstAndLast = emotePlc.split('-');
                int first = firstAndLast[0].toInt();
                int last = firstAndLast.length() > 1 ? firstAndLast[1].toInt() : first;

                emotePositionsMap.insert(first, qMakePair(last, key.toInt()));
            }
        }
    }
    return emotePositionsMap;
}

class UnicodeCharacterCounter {
public:
    UnicodeCharacterCounter(const QString s) : s(s) { }
    int toUtf16Offset(int unicodeOffset) {
        if (unicodeOffset < curUnicodeOffset) {
            curUnicodeOffset = 0;
            curQStringOffset = 0;
        }
        
        while (curUnicodeOffset < unicodeOffset) {
            if (curQStringOffset >= s.length()) return s.length();
            const QChar ch = s.at(curQStringOffset++);
            if ((0xd800 <= ch) && (ch <= 0xdbff)) {
                if (curQStringOffset >= s.length()) return s.length();
                curQStringOffset++; // consume another QString char
            }
            curUnicodeOffset++;
        }

        return curQStringOffset;
    }
private:
    int curUnicodeOffset = 0;
    int curQStringOffset = 0;
    const QString s;
};

void IrcChat::createEmoteMessageList(const QMap<int, QPair<int, int>> & emotePositionsMap, QVariantList & messageList, const QString message) {
    // cut up message into an ordered list of text fragments and emotes
    int cur = 0;
    UnicodeCharacterCounter counter(message);
    for (auto i = emotePositionsMap.constBegin(); i != emotePositionsMap.constEnd(); i++) {
        auto emoteStart = counter.toUtf16Offset(i.key());
        if (emoteStart > cur) {
            addWordSplit(message.mid(cur, emoteStart - cur), ' ', messageList);
        }
        auto emoteFinalUnicode = i.value().first;
        auto emoteAfterEnd = counter.toUtf16Offset(emoteFinalUnicode + 1);
        auto emoteId = i.value().second;
        QString emoteText = message.mid(emoteStart, emoteAfterEnd - emoteStart);
        messageList.append(createEmoteEntry(emoteId, emoteText));
        cur = emoteAfterEnd;
    }
    if (cur < message.length()) {
        addWordSplit(message.mid(cur, message.length() - cur), ' ', messageList);
    }
}

void IrcChat::parseMessageCommand(const QString cmd, const QString cmdKeyword, CommandParse & commandParse) {
    QString displayName = "";

    QString & emotesStr = commandParse.emotesStr;
    emotesStr = "";

    commandParse.tags = getTags(cmd);

    ChatMessage & chatMessage = commandParse.chatMessage;

    chatMessage.isAction = false;
    chatMessage.isChannelNotice = false;

    foreach(const QString & tagStr, commandParse.tags) {
        Tag tag(tagStr);
        if (!tag.valid) continue;
        if (tag.key == "display-name") {
            displayName = tag.value;
        }
        else if (tag.key == "color") {
            chatMessage.color = tag.value;
        }
        else if (tag.key == "subscriber") {
            chatMessage.subscriber = (tag.value == "1");
        }
        else if (tag.key == "turbo") {
            chatMessage.turbo = (tag.value == "1");
        }
        else if (tag.key == "mod") {
            chatMessage.mod = (tag.value == "1");
        }
        else if (tag.key == "badges") {
            QList<QPair<QString, QString>> badgesMap = parseBadges(tag.value);

            for (auto entry = badgesMap.constBegin(); entry != badgesMap.constEnd(); entry++) {
                makeBadgeAvailable(entry->first, entry->second);
                chatMessage.badges.push_back(QVariantList({ entry->first, entry->second }));
            }
        }
        else if (tag.key == "emotes") {
            emotesStr = tag.value;
        }
        else {
            //qDebug() << "Unused " << cmdKeyword << " tag" << tag.key;
        }
    }

    int cmdKeywordPos = cmd.indexOf(cmdKeyword);

    commandParse.params = cmd.left(cmdKeywordPos);
    QString nickname = commandParse.params.left(commandParse.params.lastIndexOf('!')).remove(0, commandParse.params.lastIndexOf(':') + 1);
    commandParse.haveMessage = false;

    commandParse.message = "";
    int messageSepPos = cmd.indexOf(':', cmdKeywordPos + cmdKeyword.length());
    if (messageSepPos != -1) {
        commandParse.haveMessage = true;
        commandParse.message = cmd.mid(messageSepPos + 1);
    }

    int channelEnd = messageSepPos == -1 ? cmd.length() : messageSepPos - 1;
    int channelStart = cmdKeywordPos + cmdKeyword.length() + 1;
    commandParse.channel = cmd.mid(channelStart, channelEnd - channelStart);

    bool isHashChannel = (commandParse.channel.length() > 0) && (commandParse.channel.at(0) == '#');
    commandParse.wrongChannel = isHashChannel && inRoom() && ((QString("#") + room) != commandParse.channel);
    if (commandParse.wrongChannel) {
        qDebug() << "message is for" << commandParse.channel.mid(1) << "and we are in" << room;
    }

    if (displayName.length() > 0) {
        chatMessage.name = displayName;
    }
    else {
        chatMessage.name = nickname;
    }

    //qDebug() << "emotes " << emotes;
}

void IrcChat::parseCommand(QString cmd) {
    if(cmd.startsWith("PING ")) {
        sock->write("PONG\r\n");
        return;
    }

    if(cmd.contains("PRIVMSG")) {

        // Structure of message: '@color=#HEX;display-name=NicK;emotes=id:start-end,start-end/id:start-end;subscriber=0or1;turbo=0or1;user-type=type :nick!nick@nick.tmi.twitch.tv PRIVMSG #channel :message'

        CommandParse parse;

        parseMessageCommand(cmd, "PRIVMSG", parse);

        if (parse.wrongChannel) {
            return;
        }

		// parse IRC action before applying emotes, as emote indices are relative to the content of the action
		const QString ACTION_PREFIX = QString(QChar(1)) + "ACTION ";
		const QString ACTION_SUFFIX = QString(QChar(1));
		if (parse.message.startsWith(ACTION_PREFIX) && parse.message.endsWith(ACTION_SUFFIX)) {
			parse.chatMessage.isAction = true;
            parse.message = parse.message.mid(ACTION_PREFIX.length(), parse.message.length() - ACTION_SUFFIX.length() - ACTION_PREFIX.length());
		}

        createEmoteMessageList(parseEmotesTag(parse.emotesStr), parse.chatMessage.messageList, parse.message);

        //qDebug() << "messageList " << messageList;

        disposeOfMessage(parse.chatMessage);
        return;
    }
    if (cmd.contains("USERNOTICE")) {
        // Structure of message: 
        // @badges=staff/1,broadcaster/1,turbo/1;color=#008000;display-name=TWITCH_UserName;emotes=;mod=0;msg-id=resub;msg-param-months=6;room-id=1337;subscriber=1;system-msg=TWITCH_UserName\shas\ssubscribed\sfor\s6\smonths!;login=twitch_username;turbo=1;user-id=1337;user-type=staff :tmi.twitch.tv USERNOTICE #channel :Great stream -- keep it up!
        // when there is no message, last part is omitted
        // @badges=staff/1,broadcaster/1,turbo/1;color=#008000;display-name=TWITCH_UserName;emotes=;mod=0;msg-id=resub;msg-param-months=6;room-id=1337;subscriber=1;system-msg=TWITCH_UserName\shas\ssubscribed\sfor\s6\smonths!;login=twitch_username;turbo=1;user-id=1337;user-type=staff :tmi.twitch.tv USERNOTICE #channel

        CommandParse parse;

        parseMessageCommand(cmd, "USERNOTICE", parse);

        if (parse.wrongChannel) {
            return;
        }

        parse.chatMessage.isChannelNotice = true;

        foreach(const QString & tagStr, parse.tags) {
            Tag tag(tagStr);
            if (tag.key == "system-msg") {
                QString systemMessage = tag.value;

                // \s -> space
                systemMessage.replace("\\s", " ");
                // double backslash -> single backslash
                systemMessage.replace("\\\\", "\\");

                parse.chatMessage.systemMessage = systemMessage;
            }
        }

        createEmoteMessageList(parseEmotesTag(parse.emotesStr), parse.chatMessage.messageList, parse.message);

        //qDebug() << "messageList " << messageList;

        disposeOfMessage(parse.chatMessage);
        return;
    }
    if(cmd.contains("NOTICE")) {
        QString text = cmd.remove(0, cmd.indexOf(':', cmd.indexOf("NOTICE")) + 1);
        emit noticeReceived(text);
    }
    if(cmd.contains("GLOBALUSERSTATE")) {
		// Structure of message: @badges=turbo/1;color=#4100CC;display-name=user_name;emote-sets=0,1,22,345;user-id=12345678;user-type= :tmi.twitch.tv GLOBALUSERSTATE
		// We want this for the emote ids
        foreach(const QString & tagStr, getTags(cmd)) {
            Tag tag(tagStr);
            if (!tag.valid) continue;
			if (tag.key == "badges") {
                badgesByChannel.remove("GLOBAL");
                auto badges = parseBadges(tag.value);

                qDebug() << "Updating user global badges from GLOBALUSERSTATE:";
                for (auto entry = badges.constBegin(); entry != badges.constEnd(); entry++) {
                    qDebug() << "  " << entry->first << ":" << entry->second;
                }

                badgesByChannel.insert("GLOBAL", badges);
                emit myBadgesForChannel("GLOBAL", badges);
			}
			else if (tag.key == "emote-sets") {
                qDebug() << "GLOBALUSERSTATE emote-sets" << tag.value;
                const QStringList & entries = tag.value.split(',');
				for (const auto & entry : entries) {
					_emoteSetIDs.append(entry.toInt());
				}
                emit emoteSetIDsChanged();
			}
            else if (tag.key == "color") {
                qDebug() << "Setting user global color to" << tag.value;
                userGlobalColor = tag.value;
            }
            else if (tag.key == "display-name") {
                userGlobalDisplayName = tag.value;
            }
            else {
                qDebug() << "Unused GLOBALUSERSTATE tag" << tag.key;
            }

		}
        return;
    }
    const QString USERSTATE_CMD = " USERSTATE ";
    if (cmd.contains(USERSTATE_CMD)) {
        //@badges=global_mod/1,turbo/1;color=#0D4200;display-name=TWITCH_UserNaME :tmi.twitch.tv USERSTATE #channel
        QString channel = cmd.mid(cmd.indexOf(USERSTATE_CMD) + USERSTATE_CMD.length());
        userChannelColors.remove(channel);
        badgesByChannel.remove(channel);
        userChannelMod.remove(channel);
        userChannelSubscriber.remove(channel);
        userChannelDisplayName.remove(channel);
        foreach(const QString & tagStr, getTags(cmd)) {
            Tag tag(tagStr);
            if (!tag.valid) continue;
            if (tag.key == "badges") {
                auto badges = parseBadges(tag.value);

                qDebug() << "Updating user badges for" << channel << "from USERSTATE:";
                for (auto entry = badges.constBegin(); entry != badges.constEnd(); entry++) {
                    qDebug() << "  " << entry->first << ":" << entry->second;
                }

                badgesByChannel.insert(channel, badges);
                emit myBadgesForChannel(channel, badges);
            }
            else if (tag.key == "color") {
                qDebug() << "Setting user color for channel" << channel << "to" << tag.value;
                userChannelColors.insert(channel, tag.value);
            }
            else if (tag.key == "mod") {
                userChannelMod.insert(channel, tag.value == "1");
            }
            else if (tag.key == "subscriber") {
                userChannelSubscriber.insert(channel, tag.value == "1");
            }
            else if (tag.key == "display-name") {
                userChannelDisplayName.insert(channel, tag.value);
            }
            else {
                qDebug() << "Unused USERSTATE tag" << tag.key;
            }
        }
        return;
    }
    //qDebug() << "Unrecognized chat command:" << cmd;
}

QString IrcChat::getParamValue(QString params, QString param) {
    QString paramValue = params.remove(0, params.indexOf(param + "="));
    paramValue = paramValue.left(paramValue.indexOf(';')).remove(0, paramValue.indexOf('=') + 1);
    return paramValue;
}

QList<int> IrcChat::emoteSetIDs() {
    return _emoteSetIDs;
}

void IrcChat::handleDownloadComplete() {
    if (allDownloadsComplete()) {
        emit downloadComplete();

        //qDebug() << "Download queue complete; posting pending messages";
        while (!msgQueue.empty()) {
            ChatMessage tmpMsg = msgQueue.first();
            emit messageReceived(tmpMsg.name, tmpMsg.messageList, tmpMsg.color, tmpMsg.subscriber, tmpMsg.turbo, tmpMsg.mod, tmpMsg.isAction, tmpMsg.badges, tmpMsg.isChannelNotice, tmpMsg.systemMessage);
            msgQueue.pop_front();
        }
    }
}

bool IrcChat::bulkDownloadEmotes(QList<QString> keys) {
    return _emoteProvider.bulkDownload(keys);
}
