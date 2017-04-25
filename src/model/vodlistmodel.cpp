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

#include "vodlistmodel.h"

VodListModel::VodListModel()
{

}

VodListModel::~VodListModel()
{
    clear();
}

Qt::ItemFlags VodListModel::flags(const QModelIndex &/*index*/) const
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

        case CreatedAt:
            var.setValue(vod->getCreatedAt());
        }
    }

    return var;
}

int VodListModel::rowCount(const QModelIndex &/*parent*/) const
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
    roles[CreatedAt] = "createdAt";
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

