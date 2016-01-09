#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QUrl>
#include "../model/channel.h"
#include "../model/game.h"

/**
 * @brief The JsonParser class
 * Handles parsing of json documents to business logic-objects
 */

class JsonParser
{
public:
    static QList<Channel*> parseStreams(const QByteArray&);
    static QList<Game*> parseGames(const QByteArray&);
    static QList<Channel*> parseChannels(const QByteArray&);
    static QList<Channel*> parseFeatured(const QByteArray&);
    static Game* parseGame(const QJsonObject&);
    static Channel* parseStream(const QByteArray&);
    static Channel* parseStream(const QJsonObject&);
    static Channel* parseChannel(const QByteArray&);
    static Channel* parseChannel(const QJsonObject&);
    static QString parseChannelStreamExtractionInfo(const QByteArray&);
};

#endif // JSONPARSER_H
