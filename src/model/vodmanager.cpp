#include "vodmanager.h"

VodManager::VodManager(NetworkManager *netman) : netman(netman)
{
    model = new VodListModel();

    connect(netman, SIGNAL(broadcastsOperationFinished(QList<Vod*>)), this, SLOT(onSearchFinished(QList<Vod*>)));
    connect(netman, SIGNAL(m3u8OperationBFinished(QStringList)), this, SLOT(onStreamGetFinished(QStringList)));
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

void VodManager::onStreamGetFinished(QStringList items)
{
    emit streamsGetFinished(items);
}

VodListModel *VodManager::getModel() const
{
    return model;
}

QString VodManager::getGame() const
{
    return game;
}

void VodManager::getBroadcasts(const QString vod)
{
    netman->getBroadcastPlaybackStream(vod);
}
