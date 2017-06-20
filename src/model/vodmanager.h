/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VODMANAGER_H
#define VODMANAGER_H

#include <QObject>
#include "../network/networkmanager.h"
#include "vodlistmodel.h"
#include <QQmlEngine>
#include <QJSEngine>

struct LastPosition {
    quint64 lastPosition;
    bool modified;
    int settingsIndex;
};


class VodManager: public QObject
{
    Q_OBJECT

    QMap<QString, QMap<QString, LastPosition>> channelVodLastPositions;

    static VodManager *instance;
    VodManager();

public:
    static VodManager *getInstance() {
        if (!instance)
            instance = new VodManager();
        return instance;
    }

    ~VodManager();

    static QObject *provider(QQmlEngine */*eng*/, QJSEngine */*jseng*/) {
        QQmlEngine::setObjectOwnership(getInstance(), QQmlEngine::CppOwnership);
        return getInstance();
    }

    Q_INVOKABLE void search(const quint64 channelId, const quint32 offset, const quint32 limit);

    Q_INVOKABLE QString getGame() const;

    Q_INVOKABLE void getBroadcasts(QString vod);

    VodListModel *getModel() const;

public slots:
    void onSearchFinished(QList<Vod *>);
    void cancelLastVodChatRequest();
    void resetVodChat();
    void getVodStartTime(quint64 vodId);
    void getVodChatPiece(quint64 vodId, quint64 offset);
    void setVodLastPlaybackPosition(const QString & channel, const QString & vod, quint64 position);
    QVariant getVodLastPlaybackPosition(const QString & channel, const QString & vod);
    QVariantMap getChannelVodsLastPlaybackPositions(const QString & channel);
    void vodLastPlaybackPositionLoaded(const QString & channel, const QString & vod, quint64 position, int settingsIndex);

signals:
    void searchStarted();
    void searchFinished();
    void streamsGetFinished(QVariantMap items);

    void vodStartGetOperationFinished(double);
    void vodChatPieceGetOperationFinished(QList<ReplayChatMessage>);
    void vodLastPositionUpdated(const QString & channel, const QString & vod, const quint64 position);

private:
    QString game;
    VodListModel *model;
    NetworkManager *netman;
};

#endif // VODMANAGER_H
