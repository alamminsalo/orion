#include "vodmanager.h"

VodManager::VodManager()
{
    vodSearch = new VodSearchOperation();
    vodGet = new VodStreamExtractOperation();
    model = new VodListModel();

    connect(vodSearch, SIGNAL(operationFinished()), this, SLOT(onSearchFinished()));
    connect(vodGet, SIGNAL(operationFinished()), this, SLOT(onStreamGetFinished()));
}

VodManager::~VodManager()
{
    delete vodGet;
    delete vodSearch;
    delete model;
}

void VodManager::search(const QString channelName, const quint32 offset, const quint32 limit)
{
    if (offset == 0) {
        model->clear();
        emit searchStarted();
    }

    vodSearch->search(channelName, offset, limit);
}

void VodManager::onSearchFinished()
{
    QList<Vod *> items = vodSearch->getResult();

    model->addAll(items);

    qDeleteAll(items);
    items.clear();

    emit searchFinished();
}

void VodManager::onStreamGetFinished()
{
    emit streamsGetFinished();
}

VodListModel *VodManager::getModel() const
{
    return model;
}

VodStreamExtractOperation *VodManager::getVodGet() const
{
    return vodGet;
}

QStringList VodManager::getResults() const
{
    return vodGet->getResult();
}

QString VodManager::getGame() const
{
    return game;
}

void VodManager::getStreams(const QString vodId)
{
    qDebug() << "Fetching vod streams for id " << vodId;
    vodGet->run(vodId);
}
