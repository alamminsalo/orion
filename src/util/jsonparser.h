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
    static QList<Channel*> parseStreams(const QJsonObject&);
    static QList<Game*> parseGames(const QJsonObject&);
    static Game* parseGame(const QJsonObject&);
    static Channel* parseStream(const QJsonObject&);
    static Channel* parseChannel(const QJsonObject&);
};

#endif // JSONPARSER_H
