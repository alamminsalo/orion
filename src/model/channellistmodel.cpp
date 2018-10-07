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

Qt::ItemFlags ChannelListModel::flags(const QModelIndex &/*index*/) const
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

int ChannelListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return channels.size();
}

void debugChannel(const QString prefix, const Channel * c) {
    qDebug() << prefix << ":" << c->getId() << c->getName() << "game" << c->getGame() << "serviceName" << c->getServiceName() << "time" << c->getTime() << "viewers" << c->getViewers();
}

void ChannelListModel::addChannelInternal(Channel *channel) {
    channels.append(channel);
    auto id = channel->getId();
    if (id == 0) {
        qDebug() << "inserting new channel with 0 id";
    }
    else {
        channelIdIndex.insert(id, channel);
    }
}

void ChannelListModel::addChannel(Channel *channel)
{
    if (updateChannelIfExisting(channel)) {
        qDebug() << "ChannelListModel::addChannel got existing channel" << channel->getId() << channel->getName();
    }
    else {
        beginInsertRows(QModelIndex(), channels.size(), channels.size());
        addChannelInternal(channel);
        endInsertRows();
    }
}

bool ChannelListModel::updateChannelIfExisting(const Channel * channel) {
    const auto id = channel->getId();

    const auto existingChannelEntry = channelIdIndex.find(id);
    if (existingChannelEntry != channelIdIndex.end()) {
        Channel * existingChannel = existingChannelEntry.value();
        debugChannel("existingChannel", existingChannel);
        debugChannel("addedChannel", channel);

        if (channel->getTime() < existingChannel->getTime()) {
            qDebug() << "added item was older; skipping update";
        }
        else {
            existingChannel->updateWith(*channel);
            updateChannelForView(existingChannel);
        }
        return true;
    }
    else {
        return false;
    }

}

int ChannelListModel::addAll(const QList<Channel *> &list)
{
    // prefilter the list to catch duplicates and update them
    QList<Channel *> filteredList;
    filteredList.reserve(list.size());

    for (Channel * channel : list) {
        if (updateChannelIfExisting(channel)) {
            qDebug() << "ChannelListModel::addAll got existing channel" << channel->getId() << channel->getName();
        }
        else {
            filteredList.append(channel);
        }
    }

    if (!filteredList.isEmpty()){
        int newSize = channels.size() + filteredList.size();
        channels.reserve(newSize);
        channelIdIndex.reserve(newSize);
        beginInsertRows(QModelIndex(), channels.size(), channels.size() + filteredList.size() - 1);
        for (const Channel* channel : filteredList){
            addChannelInternal(new Channel(*channel));
        }
        endInsertRows();
    }

    return filteredList.length();
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
        beginRemoveRows(QModelIndex(), index, index);
        auto indexEntry = channelIdIndex.find(channel->getId());
        if (indexEntry != channelIdIndex.end() && indexEntry.value() == channel) {
            channelIdIndex.erase(indexEntry);
        }
        delete channels.takeAt(index);
        endRemoveRows();
    }
}

Channel *ChannelListModel::find(const quint32 &id)
{
    auto indexEntry = channelIdIndex.find(id);
    if (indexEntry != channelIdIndex.end()) {
        return indexEntry.value();
    }
    return nullptr;
}

void ChannelListModel::clearView()
{
    //Gives a sign to drop all channels from view, without removing them
    if (!channels.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, channels.size() - 1);
        endRemoveRows();
    }
}

void ChannelListModel::clear()
{
    if (!channels.isEmpty()){
        beginRemoveRows(QModelIndex(), 0, channels.size() - 1);
        qDeleteAll(channels);
        channels.clear();
        channelIdIndex.clear();
        endRemoveRows();
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
    if (item){
        if (item->getId() == 0) {
            qDebug() << "updateChannel with id==0 item";
            return;
        }

        if (Channel *channel = find(item->getId())){
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

bool ChannelListModel::updateStream(Channel *item)
{
    bool onlineStateChanged = false;

    if (item && item->getId()){

        if (Channel *channel = find(item->getId())){

            if (item->isOnline()){
                channel->setViewers(item->getViewers());
                channel->setGame(item->getGame());
                channel->setPreviewurl(item->getPreviewurl());

                if (!item->getName().isEmpty()){
                    updateChannel(item);
                }
            }

            if (!item->isOnline()) {
                // put some information back for notifications
                if (!channel->getName().isEmpty()) {
                    item->setName(channel->getName());
                }
            }

            if (channel->isOnline() != item->isOnline()){
                channel->setOnline(item->isOnline());
                updateChannelForView(channel);

                onlineStateChanged = true;

                //emit channelOnlineStateChanged(channel);
            }
        }
    }
    else {
        qDebug() << "ChannelListModel::updateStream got an item without id" << item->getServiceName();
    }

    return onlineStateChanged;
}

void ChannelListModel::setAllChannelsOffline()
{
    int i = 0;
    foreach(Channel *channel, channels) {
        if (channel->isOnline()) {
            channel->setOnline(false);
            //updateChannelForView(channel);
            emit dataChanged(index(i), index(i));
            emit channelOnlineStateChanged(channel);
        }
        i++;
    }
}

void ChannelListModel::updateStreams(const QList<Channel *> &list)
{
    QList<Channel*> onlineChannels;
    QList<Channel*> offlineChannels;

    if (!channels.isEmpty()){
        foreach(Channel *channel, list){
            if (updateStream(channel)) {
                //Channel online status has changed

                if (channel->isOnline())
                    //Channel is up, add to changed online channels
                    onlineChannels << channel;

                else
                    emit channelOnlineStateChanged(channel);
            }
        }
    }

    //Handle online channels
    if (!onlineChannels.isEmpty()) {
        qDebug() << "Multiple channels online!";
        emit multipleChannelsChangedOnline(onlineChannels);
    }

    //emit channelsUpdated();
}



