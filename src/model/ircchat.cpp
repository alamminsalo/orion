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

const QString IMAGE_PROVIDER_EMOTE = "emote";
const QString EMOTICONS_URL_FORMAT = "https://static-cdn.jtvnw.net/emoticons/v1/%1/1.0";

IrcChat::IrcChat(QObject *parent) :
    QObject(parent) {

    logged_in = false;

    // Open socket
    sock = new QTcpSocket(this);
    if(sock) {
        emit errorOccured("Error opening socket");
    }

    //createConnection();
    connect(sock, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));
    connect(sock, SIGNAL(connected()), this, SLOT(login()));
    connect(sock, SIGNAL(connected()), this, SLOT(onSockStateChanged()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(onSockStateChanged()));

    room = "";

	emoteDir = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QString("/emotes"));
	emoteDirPathImpl = "image://" + IMAGE_PROVIDER_EMOTE;

	activeDownloadCount = 0;
}

void IrcChat::initProviders() {
	auto engine = qmlEngine(this);
	RegisterEngineProviders(*engine);
}

void IrcChat::RegisterEngineProviders(QQmlEngine & engine) {
	auto provider = new CachedImageProvider(_emoteTable);
	engine.addImageProvider(IMAGE_PROVIDER_EMOTE, provider);
}

IrcChat::~IrcChat() {
    disconnect();
    for (auto i = _emoteTable.begin(); i != _emoteTable.end(); i++ ) {
        QImage * image = i.value();
        i.value() = NULL;
        if (image != NULL) {
            delete image;
        }
    }
}

void IrcChat::join(const QString channel) {

    if (inRoom())
        leave();

    // Save channel name for later use
    room = channel;

    if (!connected()) {
        reopenSocket();
    }

    // Join channel's chat room
    sock->write(("JOIN #" + channel + "\r\n").toStdString().c_str());

    qDebug() << "Joined channel " << channel;
}

void IrcChat::leave()
{
    sock->write(("PART #" + room + "\r\n").toStdString().c_str());
    room = "";
}

void IrcChat::disconnect() {
    leave();
    sock->close();
}

void IrcChat::reopenSocket() {
    qDebug() << "Reopening socket";
    if(sock->isOpen())
        sock->close();
    sock->open(QIODevice::ReadWrite);
    sock->connectToHost(HOST, PORT);
    if(!sock->isOpen()) {
        errorOccured("Error opening socket");
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
    return sock->state() == QTcpSocket::ConnectedState;
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
            makeEmoteAvailable(QString::number(emoteId));
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
            list.erase(it);
        }
        else {
            ++it;
        }
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
            badges.push_back(QVariantList({ badge->first, badge->second }));
        }
        return true;
    }
    else {
        return false;
    }
}

void IrcChat::sendMessage(const QString &msg, const QVariantMap &relevantEmotes) {
    if (inRoom() && connected()) {
        sock->write(("PRIVMSG #" + room + " :" + msg + "\r\n").toStdString().c_str());

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

void IrcChat::createConnection()
{
    sock->connectToHost(HOST, PORT);
}

void IrcChat::login()
{
    if (userpass.isEmpty() || username.isEmpty())
        setAnonymous(true);
    else
        setAnonymous(false);

    // Tell server that we support twitch-specific commands
    sock->write("CAP REQ :twitch.tv/commands\r\n");
    sock->write("CAP REQ :twitch.tv/tags\r\n");

    // Login
    sock->write(("PASS " + userpass + "\r\n").toStdString().c_str());
    sock->write(("NICK " + username + "\r\n").toStdString().c_str());

    logged_in = true;

    //Join room automatically, if given
    if (!room.isEmpty())
        join(room);
}

void IrcChat::receive() {
    QString msg;
    while (sock->canReadLine()) {
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

    errorOccured(err);
}

void IrcChat::addWordSplit(const QString & s, const QChar & sep, QVariantList & l) {
	bool first = true;
	for (auto part : s.split(sep)) {
		if (first) {
			first = false;
			l.append(part);
		}
		else {
			l.append(QString(sep) + part);
		}
	}
}

void IrcChat::disposeOfMessage(ChatMessage m) {
    if (activeDownloadCount == 0) {
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

            makeEmoteAvailable(key);

            for (auto emotePlc : positions.split(',')) {
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

struct CommandParse {
    ChatMessage chatMessage;
    QString params;
    bool haveMessage;
    QString message;
    QList<QString> tags;
    QString emotesStr;
};

void parseMessageCommand(const QString cmd, const QString cmdKeyword, CommandParse & commandParse) {
    QString displayName = "";
    /*
    QString color = "";
    bool subscriber = false;
    bool turbo = false;
    bool mod = false;
    */

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

        parse.chatMessage.isChannelNotice = true;

        foreach(const QString & tagStr, parse.tags) {
            Tag tag(tagStr);
            if (tag.key == "system-msg") {
                QString systemMessage = tag.value;

                // \s -> space
                systemMessage.replace("\\s", " ");
                // \\ -> \ 
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
				for (auto entry : tag.value.split(',')) {
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

bool IrcChat::downloadEmotes(QString key) {
    if(_emoteTable.contains(key)) {
      qDebug() << "already in the table";
        return false;
    }
    
    QUrl url = EMOTICONS_URL_FORMAT.arg(key);
    emoteDir.mkpath(".");

    QString filename = emoteDir.absoluteFilePath(key + ".png");

    if(emoteDir.exists(key + ".png")) {
        //qDebug() << "local file already exists";
		loadEmoteImageFile(key, filename);
        return false;
    }
	qDebug() << "downloading";

    QNetworkRequest request(url);
	QNetworkReply* _reply = nullptr;
    _reply = _manager.get(request);

	DownloadHandler * dh = new DownloadHandler(filename);

    connect(_reply, &QNetworkReply::readyRead,
      dh, &DownloadHandler::dataAvailable);
    connect(_reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
      dh, &DownloadHandler::error);
    connect(_reply, &QNetworkReply::finished,
      dh, &DownloadHandler::replyFinished);
	connect(dh, &DownloadHandler::downloadComplete,
		this, &IrcChat::individualDownloadComplete);

    return true;
}

bool IrcChat::makeEmoteAvailable(QString key) {
    /* Make emote available by downloading it or loading it from cache if not already loaded.
     * Returns true if caller should wait for a downloadComplete event before using the emote */
    if (emotesCurrentlyDownloading.contains(key)) {
        // download of this emote in progress
        return true;
    }
    else if (downloadEmotes(key)) {
        // if this emote isn't already downloading, it's safe to load the cache file or download if not in the cache
        emotesCurrentlyDownloading.insert(key);
        activeDownloadCount += 1;
        return true;
    }
    else {
        // we already had the emote locally and don't need to wait for it to download
        return false;
    }
}

bool IrcChat::bulkDownloadEmotes(QList<QString> emoteIDs) {
    bool waitForDownloadComplete = false;
    for (auto key : emoteIDs) {
        if (makeEmoteAvailable(key)) {
            waitForDownloadComplete = true;
        }
    }
    return waitForDownloadComplete;
}

CachedImageProvider::CachedImageProvider(QHash<QString, QImage*> & imageTable) : QQuickImageProvider(QQuickImageProvider::Image), imageTable(imageTable) {

}

QImage CachedImageProvider::requestImage(const QString &id, QSize * size, const QSize & requestedSize) {
	//qDebug() << "Requested id" << id << "from image provider";
	QImage * entry = NULL;
	auto result = imageTable.find(id);
	if (result != imageTable.end()) {
		entry = *result;
	}
	if (entry) {
		if (size) {
			*size = entry->size();
		}
		return *entry;
	}
	return QImage();
}

DownloadHandler::DownloadHandler(QString filename) : filename(filename), hadError(false) {
    _file.setFileName(filename);
    _file.open(QFile::WriteOnly);
	qDebug() << "starting download of" << filename;
}

void DownloadHandler::dataAvailable() {
  QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
  auto buffer = _reply->readAll();
  _file.write(buffer.data(), buffer.size());
}

void DownloadHandler::error(QNetworkReply::NetworkError code) {
  hadError = true;
  QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
  qDebug() << "Network error downloading" << filename << ":" << _reply->errorString();
}

void DownloadHandler::replyFinished() {
  QNetworkReply* _reply = qobject_cast<QNetworkReply*>(sender());
  if(_reply) {
    _reply->deleteLater();
	_file.close();
    //qDebug() << _file.fileName();
    //might need something for windows for the forwardslash..
    qDebug() << "download of" << _file.fileName() << "complete";

    emit downloadComplete(_file.fileName(), hadError);
  }
}

void IrcChat::loadEmoteImageFile(QString emoteKey, QString filename) {
    QImage* emoteImg = new QImage();
    //qDebug() << "loading" << filename;
    emoteImg->load(filename);
    _emoteTable.insert(emoteKey, emoteImg);
}

QList<int> IrcChat::emoteSetIDs() {
    return _emoteSetIDs;
}

void IrcChat::individualDownloadComplete(QString filename, bool hadError) {
    DownloadHandler * dh = qobject_cast<DownloadHandler*>(sender());
    delete dh;
    
	QString emoteKey = filename.left(filename.indexOf(".png")).remove(0, filename.lastIndexOf('/') + 1);
	if (hadError) {
		// delete partial download if any
		QFile(filename).remove();
	} else {
		loadEmoteImageFile(emoteKey, filename);
	}
    
	if (activeDownloadCount > 0) {
		activeDownloadCount--;
		qDebug() << activeDownloadCount << "active downloads remaining";
	}
    
	emotesCurrentlyDownloading.remove(emoteKey);

	if (activeDownloadCount == 0) {
        emit downloadComplete();
        //qDebug() << "Download queue complete; posting pending messages";
		while (!msgQueue.empty()) {
			ChatMessage tmpMsg = msgQueue.first();
			emit messageReceived(tmpMsg.name, tmpMsg.messageList, tmpMsg.color, tmpMsg.subscriber, tmpMsg.turbo, tmpMsg.mod, tmpMsg.isAction, tmpMsg.badges, tmpMsg.isChannelNotice, tmpMsg.systemMessage);
			msgQueue.pop_front();
		}
	}
}

QHash<QString, QImage*> IrcChat::emoteTable() {
  return _emoteTable;
}
