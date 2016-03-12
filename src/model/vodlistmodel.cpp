#include "vodlistmodel.h"

VodListModel::VodListModel()
{

}

VodListModel::~VodListModel()
{
    clear();
}

Qt::ItemFlags VodListModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled;
}

QVariant VodListModel::data(const QModelIndex &index, int role) const
{
    QVariant var;

    if (!index.isValid()){
        return var;
    }

    Vod *vod = vods.at(index.row());

    if (vod){
        switch(role){
        case Title:
            var.setValue(vod->getTitle());
            break;

        case Duration:
            var.setValue(vod->getDuration());
            break;

        case Preview:
            var.setValue(vod->getPreview());
            break;

        case Views:
            var.setValue(vod->getViews());
            break;

        case Id:
            var.setValue(vod->getId());
            break;

        case Game:
            var.setValue(vod->getGame());
            break;
        }
    }

    return var;
}

int VodListModel::rowCount(const QModelIndex &parent) const
{
    return vods.size();
}

QHash<int, QByteArray> VodListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Title] = "title";
    roles[Preview] = "preview";
    roles[Id] = "id";
    roles[Game] = "game";
    roles[Duration] = "duration";
    roles[Views] = "views";
    return roles;
}

void VodListModel::addAll(QList<Vod *> &items)
{
    if (!items.isEmpty()){
        emit beginInsertRows(QModelIndex(), vods.size(), vods.size() + items.size() - 1);
        foreach (Vod *vod, items) {
            vods.append(new Vod(*vod));
        }
        emit endInsertRows();
    }
}

Vod *VodListModel::find(const QString id)
{
    foreach(Vod *vod, vods)
        if (vod->getId() == id)
            return vod;
    return 0;
}

void VodListModel::clear()
{
    if (!vods.isEmpty()){
        emit beginRemoveRows(QModelIndex(), 0, vods.size());
        qDeleteAll(vods);
        vods.clear();
        emit endRemoveRows();
    }
}

int VodListModel::count() const
{
    return rowCount();
}

