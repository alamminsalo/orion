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
    int row = index.row();
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

void ChannelListModel::removeChannel(Channel *channel)
{
    if (int index = channels.indexOf(channel) > -1){
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

void ChannelListModel::clear()
{
    emit beginRemoveRows(QModelIndex(), 0, channels.size());
    if (!channels.isEmpty()){
        qDeleteAll(channels);
        channels.clear();
    }
    emit endRemoveRows();
}

void ChannelListModel::updateChannelForView(Channel* channel)
{
    int i = channels.indexOf(channel);

    if (i > -1){
        emit dataChanged(index(i), index(i));
    }
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

    return roles;
}

void ChannelListModel::updateChannel(Channel *item)
{
    if (item && !item->getServiceName().isEmpty()){

        if (Channel *channel = find(item->getServiceName())){

            channel->setName(item->getName());
            channel->setLogourl(item->getLogourl());
            channel->setLogoPath(item->getLogoPath());
            channel->setInfo(item->getInfo());
            updateChannelForView(channel);
            //channel->updated();
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

    //emit channelsUpdated();
}

void ChannelListModel::updateStream(Channel *item)
{
    if (item && !item->getServiceName().isEmpty()){

        if (Channel *channel = find(item->getServiceName())){

            if (item->isOnline()){
                channel->setViewers(item->getViewers());
                channel->setGame(item->getGame());
                channel->setPreviewurl(item->getPreviewurl());
                channel->setPreviewPath(item->getPreviewPath());

                if (!item->getName().isEmpty()){
                    updateChannel(item);
                }
            } /*else if (channel->getName().isEmpty()){
                netman->getChannel(channel->getServiceName());
            }*/

            if (channel->isOnline() != item->isOnline()){
                channel->setOnline(item->isOnline());
                updateChannelForView(channel);
                emit channelOnlineStateChanged(channel);
                //channel->updated();
            }
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



