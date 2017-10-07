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
#include "singletonprovider.h"

struct LastPosition {
    quint64 lastPosition;
    bool modified;
    int settingsIndex;
};

class VodManager: public QObject
{
    QML_SINGLETON
    Q_OBJECT

    Q_PROPERTY(VodListModel model READ getModel NOTIFY modelChanged)

    QMap<QString, QMap<QString, LastPosition>> channelVodLastPositions;

    static VodManager *instance;
    explicit VodManager(QObject *parent = 0);

public:
    static VodManager *getInstance();

    ~VodManager();

    Q_INVOKABLE void search(const quint64 channelId, const quint32 offset, const quint32 limit);

    Q_INVOKABLE QString getGame() const;

    Q_INVOKABLE void getBroadcasts(QString vod);

    VodListModel *getModel() const;

public slots:
    void onSearchFinished(QList<Vod *>);
    void cancelLastVodChatRequest();
    void resetVodChat();
    void getVodChatPiece(quint64 vodId, quint64 offset);
    void getNextVodChatPiece(quint64 vodId, QString cursor);
    void setVodLastPlaybackPosition(const QString & channel, const QString & vod, quint64 position);
    QVariant getVodLastPlaybackPosition(const QString & channel, const QString & vod);
    QVariantMap getChannelVodsLastPlaybackPositions(const QString & channel);
    void vodLastPlaybackPositionLoaded(const QString & channel, const QString & vod, quint64 position, int settingsIndex);

signals:
    void modelChanged();
    void searchStarted();
    void searchFinished();
    void streamsGetFinished(QVariantMap items);

    void vodChatPieceGetOperationFinished(ReplayChatPiece);
    void vodLastPositionUpdated(const QString & channel, const QString & vod, const quint64 position);

private:
    QString game;
    VodListModel *_model;
    NetworkManager *netman;
};

#endif // VODMANAGER_H
