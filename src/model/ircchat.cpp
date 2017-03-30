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

IrcChat::~IrcChat() { disconnect(); }

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

void IrcChat::sendMessage(const QString &msg) {
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
        //TODO need the user's status info to show here
        emit messageReceived(username, message, "", false, false, isAction);
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

void IrcChat::parseCommand(QString cmd) {
    if(cmd.startsWith("PING ")) {
        sock->write("PONG\r\n");
        return;
    }
    if(cmd.contains("PRIVMSG")) {

        // Structure of message: '@color=#HEX;display-name=NicK;emotes=id:start-end,start-end/id:start-end;subscriber=0or1;turbo=0or1;user-type=type :nick!nick@nick.tmi.twitch.tv PRIVMSG #channel :message'

        QString displayName = "";
        QString color = "";
        bool subscriber = false;
        bool turbo = false;
        QString emotes = "";

        if (cmd.at(0) == QChar('@')) {
            // tags are present
            int tagsEnd = cmd.indexOf(" ");
            QString tags = cmd.mid(1, tagsEnd - 1);
            foreach(const QString & tag, tags.split(";")) {
                int assignPos = tag.indexOf("=");
                if (assignPos == -1) continue;
                QString key = tag.left(assignPos);
                QString value = tag.mid(assignPos + 1);
                if (key == "display-name") {
                    displayName = value;
                }
                else if (key == "color") {
                    color = value;
                }
                else if (key == "subscriber") {
                    subscriber = (value == "1");
                }
                else if (key == "turbo") {
                    turbo = (value == "1");
                }
				else if (key == "emotes") {
					emotes = value;
				}
            }
        }

        QString params = cmd.left(cmd.indexOf("PRIVMSG"));
        QString nickname = params.left(params.lastIndexOf('!')).remove(0, params.lastIndexOf(':') + 1);
        QString message = cmd.remove(0, cmd.indexOf(':', cmd.indexOf("PRIVMSG")) + 1);
        QString oldmessage = cmd.remove(0, cmd.indexOf(':', cmd.indexOf("PRIVMSG")) + 1);
        //qDebug() << "emotes " << emotes;
        //qDebug() << "oldmessage " << oldmessage;

        QMap<int, QPair<int, int>> emotePositionsMap;

        if(emotes != "") {
          auto emoteList = emotes.split('/');

          for(auto emote : emoteList) {
            auto key = emote.left(emote.indexOf(':'));
            auto positions = emote.remove(0, emote.indexOf(':')+1);
            //qDebug() << "key " << key;
			if (!emotesCurrentlyDownloading.contains(key)) {
				// if this emote isn't already downloading, it's safe to load the cache file or download if not in the cache
                if (downloadEmotes(key)) {
					emotesCurrentlyDownloading.insert(key);
					activeDownloadCount += 1;
				}
			}
			else {
				qDebug() << "download of " << key << " already in progress";
			}
			for(auto emotePlc : positions.split(',')) {
              auto firstAndLast = emotePlc.split('-');
              int first = firstAndLast[0].toInt();
              int last = firstAndLast.length() > 1 ? firstAndLast[1].toInt() : first;

              emotePositionsMap.insert(first, qMakePair(last, key.toInt()));
            }
          }
        }

		bool isAction = false;
		
		// parse IRC action before applying emotes, as emote indices are relative to the content of the action
		const QString ACTION_PREFIX = QString(QChar(1)) + "ACTION ";
		const QString ACTION_SUFFIX = QString(QChar(1));
		if (message.startsWith(ACTION_PREFIX) && message.endsWith(ACTION_SUFFIX)) {
			isAction = true;
			message = message.mid(ACTION_PREFIX.length(), message.length() - ACTION_SUFFIX.length() - ACTION_PREFIX.length());
		}

		// cut up message into an ordered list of text fragments and emotes
        QVariantList messageList;

        int cur = 0;
        for (auto i = emotePositionsMap.constBegin(); i != emotePositionsMap.constEnd(); i++) {
            auto emoteStart = i.key();
            if (emoteStart > cur) {
				addWordSplit(message.mid(cur, emoteStart - cur), ' ', messageList);
            }
            auto emoteEnd = i.value().first;
            auto emoteId = i.value().second;
            messageList.append(emoteId);
            cur = emoteEnd + 1;
        }
        if (cur < message.length()) {
			addWordSplit(message.mid(cur, message.length() - cur), ' ', messageList);
        }

        //qDebug() << "messageList " << messageList;
        if (displayName.length() > 0) {
            nickname = displayName;
        }

        if(activeDownloadCount == 0) {
          emit messageReceived(nickname, messageList, color, subscriber, turbo, isAction);
        }
        else {
          // queue message to be shown when downloads are complete
		  msgQueue.push_back({nickname, messageList, color, subscriber, turbo, isAction});
        }
        return;
    }
    if(cmd.contains("NOTICE")) {
        QString text = cmd.remove(0, cmd.indexOf(':', cmd.indexOf("NOTICE")) + 1);
        emit noticeReceived(text);
    }
    if(cmd.contains("GLOBALUSERSTATE")) {
		// Structure of message: @badges=turbo/1;color=#4100CC;display-name=user_name;emote-sets=0,1,22,345;user-id=12345678;user-type= :tmi.twitch.tv GLOBALUSERSTATE
		// We want this for the emote ids
		if (cmd.at(0) == QChar('@')) {
			int tagsEnd = cmd.indexOf(' ');
			QString tags = cmd.mid(1, tagsEnd - 1);
            foreach(const QString & tag, tags.split(";")) {
                int assignPos = tag.indexOf("=");
                if (assignPos == -1) continue;
				QString key = tag.left(assignPos);
				QString value = tag.mid(assignPos + 1);
				if (key == "badges") {
					for (auto badge : value.split(',')) {
						auto badgePair = badge.split('/');
						if (badgePair.length() < 2) continue;
						QString badgeName = badgePair[0];
						QString badgeNum = badgePair[1];
						badges.insert(badgeName, badgeNum);
					}
				}
				else if (key == "emote-sets") {
                    qDebug() << "GLOBALUSERSTATE emote-sets" << value;
					for (auto entry : value.split(',')) {
						_emoteSetIDs.append(entry.toInt());
					}
                    emit emoteSetIDsChanged();
				}

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

bool IrcChat::bulkDownloadEmotes(QList<QString> emoteIDs) {
    bool waitForDownloadComplete = false;
    for (auto key : emoteIDs) {
        if (emotesCurrentlyDownloading.contains(key)) {
            // download of this emote in progress
            waitForDownloadComplete = true;
        } else if (downloadEmotes(key)) {
            // if this emote isn't already downloading, it's safe to load the cache file or download if not in the cache
            emotesCurrentlyDownloading.insert(key);
            activeDownloadCount += 1;
            waitForDownloadComplete = true;
        } else {
            // we already had the emote locally and don't need to wait for it to download
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
			emit messageReceived(tmpMsg.name, tmpMsg.messageList, tmpMsg.color, tmpMsg.subscriber, tmpMsg.turbo, tmpMsg.isAction);
			msgQueue.pop_front();
		}
	}
}

QHash<QString, QImage*> IrcChat::emoteTable() {
  return _emoteTable;
}
