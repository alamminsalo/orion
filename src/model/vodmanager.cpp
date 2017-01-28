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

#include "vodmanager.h"

VodManager::VodManager(NetworkManager *netman) : netman(netman)
{
    model = new VodListModel();

    connect(netman, SIGNAL(broadcastsOperationFinished(QList<Vod*>)), this, SLOT(onSearchFinished(QList<Vod*>)));
    connect(netman, SIGNAL(m3u8OperationBFinished(QVariantMap)), this, SIGNAL(streamsGetFinished(QVariantMap)));
}

VodManager::~VodManager()
{
    delete model;
}

void VodManager::search(const QString channelName, const quint32 offset, const quint32 limit)
{
    if (offset == 0) {
        model->clear();
        emit searchStarted();
    }

    netman->getBroadcasts(channelName, offset, limit);
}

void VodManager::onSearchFinished(QList<Vod *> items)
{
    model->addAll(items);

    qDeleteAll(items);
    items.clear();

    emit searchFinished();
}

VodListModel *VodManager::getModel() const
{
    return model;
}

QString VodManager::getGame() const
{
    return game;
}

void VodManager::getBroadcasts(QString vod)
{
    //Remove leading NaN characters
    vod.remove(QRegExp("[^0-9]"));

    netman->getBroadcastPlaybackStream(vod);
}
