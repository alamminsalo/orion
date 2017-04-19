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

#ifndef VODLISTMODEL_H
#define VODLISTMODEL_H

#include <QAbstractListModel>
#include <QHashData>
#include <QList>
#include <QDebug>
#include "vod.h"

class VodListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    VodListModel();
    ~VodListModel();

    enum Roles {
        Title =          Qt::UserRole + 1,
        Preview,
        Id,
        Game,
        Duration,
        Views,
        CreatedAt
    };

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int,QByteArray> roleNames() const;

    void addAll(QList<Vod *> &items);
    Vod *find(const QString id);
    void clear();

    Q_INVOKABLE int count() const;

private:
    QList<Vod*> vods;
};

#endif // VODLISTMODEL_H
