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

#ifndef CHANNELCOLLECTION_H
#define CHANNELCOLLECTION_H

#include <QAbstractListModel>
#include <QHashData>
#include <QList>
#include "channel.h"

class ChannelListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    ChannelListModel();
    ~ChannelListModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int,QByteArray> roleNames() const;

    void updateChannel(Channel*);
    void updateChannels(const QList<Channel*>&);
    void updateStreams(const QList<Channel*>&);
    void updateStream(Channel*);
    void checkStreams();

    void setAllChannelsOffline();

    void addChannel(Channel*);
    void addAll(const QList<Channel*> &);
    void mergeAll(const QList<Channel*> &);

    void removeChannel(Channel*);
    Channel* find(const QString&);
    Channel* find(const quint32&);
    void clearView();
    void clear();

    enum Roles {
        NameRole =          Qt::UserRole + 1,
        InfoRole =          Qt::UserRole + 2,
        LogoRole =          Qt::UserRole + 3,
        PreviewRole =       Qt::UserRole + 4,
        OnlineRole =        Qt::UserRole + 5,
        ViewersRole =       Qt::UserRole + 6,
        ServiceNameRole =   Qt::UserRole + 7,
        GameRole =          Qt::UserRole + 8,
        IdRole =            Qt::UserRole + 9,
        FavouriteRole =     Qt::UserRole + 10
    };

    QList<Channel *> getChannels() const;

public slots:
    void updateChannelForView(Channel*);
    int count();

signals:
    void channelOnlineStateChanged(Channel*);

private:
    Q_DISABLE_COPY(ChannelListModel)
    QList<Channel*> channels;

};

#endif // CHANNELCOLLECTION_H
