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

#include "channellistmodel.h"

ChannelListModel::ChannelListModel()
{
}

ChannelListModel::~ChannelListModel()
{
    qDebug() << "Destroyer: ChannelListModel";
    clear();
}

Qt::ItemFlags ChannelListModel::flags(const QModelIndex &index) const
{
    //int row = index.row();
    //    Channel* channel = source.at(index);
    //    if (channel->online)
    return Qt::ItemIsEnabled;
}

QVariant ChannelListModel::data(const QModelIndex &index, int role) const
{
    QVariant var;

    if (!index.isValid()){
        return var;
    }

    Channel *channel = channels.at(index.row());

    if (channel){
        switch(role){
        case NameRole:
            var.setValue(channel->getName());
            break;

        case InfoRole:
            var.setValue(channel->getInfo());
            break;

        case LogoRole:
            var.setValue(channel->getLogourl());
            break;

        case PreviewRole:
            var.setValue(channel->getPreviewurl());
            break;

        case OnlineRole:
            var.setValue(channel->isOnline());
            break;

        case ViewersRole:
            var.setValue(channel->getViewers());
            break;

        case ServiceNameRole:
            var.setValue(channel->getServiceName());
            break;

        case GameRole:
            var.setValue(channel->getGame());
            break;

        case IdRole:
            var.setValue(channel->getId());
            break;

        case FavouriteRole:
            var.setValue(channel->isFavourite());
            break;
        }
    }

    return var;
}

int ChannelListModel::rowCount(const QModelIndex &parent) const
{
    return channels.size();
}

void ChannelListModel::addChannel(Channel *channel)
{
    emit beginInsertRows(QModelIndex(), channels.size(), channels.size());
    channels.append(channel);
    emit endInsertRows();
}

void ChannelListModel::addAll(const QList<Channel *> &list)
{
    if (!list.isEmpty()){
        emit beginInsertRows(QModelIndex(), channels.size(), channels.size() + list.size() - 1);
        foreach (Channel* channel, list){
            channels.append(new Channel(*channel));
        }

        emit endInsertRows();
    }
}

void ChannelListModel::mergeAll(const QList<Channel *> &list)
{
    if (!list.isEmpty()){
        foreach (Channel* channel, list){
            Channel *c = find(channel->getId());
            if (c) {
                c->updateWith(*channel);
            } else {
                addChannel(new Channel(*channel));
            }
        }
    }
}

void ChannelListModel::removeChannel(Channel *channel)
{
    int index = channels.indexOf(channel);
    if (index > -1){
        emit beginRemoveRows(QModelIndex(), index, index);
        delete channels.takeAt(index);
        emit endRemoveRows();
    }
}

Channel *ChannelListModel::find(const QString &q)
{
    foreach(Channel *channel, channels){
        if (channel->getServiceName() == q){
            return channel;
        }
    }
    return 0;
}

Channel *ChannelListModel::find(const quint32 &id)
{
    foreach(Channel *channel, channels){
        if (channel->getId() == id){
            return channel;
        }
    }
    return 0;
}

void ChannelListModel::clearView()
{
    //Gives a sign to drop all channels from view, without removing them
    emit beginRemoveRows(QModelIndex(), 0, channels.size());
    emit endRemoveRows();
}

void ChannelListModel::clear()
{
    if (!channels.isEmpty()){
        emit beginRemoveRows(QModelIndex(), 0, channels.size());
        qDeleteAll(channels);
        channels.clear();
        emit endRemoveRows();
    }
}

void ChannelListModel::updateChannelForView(Channel* channel)
{
    if (channel){
        int i = channels.indexOf(channel);
        if (i > -1){
            emit dataChanged(index(i), index(i));
        }
    }
}

int ChannelListModel::count()
{
    return rowCount();
}

QList<Channel *> ChannelListModel::getChannels() const
{
    return channels;
}

QHash<int, QByteArray> ChannelListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[NameRole] =  "name";
    roles[InfoRole] = "info";
    roles[LogoRole] = "logo";
    roles[PreviewRole] = "preview";
    roles[OnlineRole] = "online";
    roles[ViewersRole] = "viewers";
    roles[ServiceNameRole] = "serviceName";
    roles[GameRole] = "game";
    roles[IdRole] = "id";
    roles[FavouriteRole] = "favourite";

    return roles;
}

void ChannelListModel::updateChannel(Channel *item)
{
    if (item && !item->getServiceName().isEmpty()){

        if (Channel *channel = find(item->getServiceName())){
            channel->updateWith(*item);
            updateChannelForView(channel);
        }
    }
}

void ChannelListModel::updateChannels(const QList<Channel *> &list)
{
    if (!channels.isEmpty()){
        foreach(Channel *channel, list){
            updateChannel(channel);
        }
    }
}

void ChannelListModel::updateStream(Channel *item)
{
    if (item && !item->getServiceName().isEmpty()){

        if (Channel *channel = find(item->getServiceName())){

            if (item->isOnline()){
                channel->setViewers(item->getViewers());
                channel->setGame(item->getGame());
                channel->setPreviewurl(item->getPreviewurl());

                if (!item->getName().isEmpty()){
                    updateChannel(item);
                }
            }

            if (channel->isOnline() != item->isOnline()){
                channel->setOnline(item->isOnline());
                updateChannelForView(channel);
                emit channelOnlineStateChanged(channel);
            }
        }
    }
}

void ChannelListModel::setAllChannelsOffline()
{
    foreach(Channel *channel, channels) {
        if (channel->isOnline()) {
            channel->setOnline(false);
            updateChannelForView(channel);
            emit channelOnlineStateChanged(channel);
        }
    }
}

void ChannelListModel::updateStreams(const QList<Channel *> &list)
{
    if (!channels.isEmpty()){
        foreach(Channel *channel, list){
            updateStream(channel);
        }
    }

    //emit channelsUpdated();
}



