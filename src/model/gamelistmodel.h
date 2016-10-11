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

#ifndef GAMELISTMODEL_H
#define GAMELISTMODEL_H

#include <QAbstractListModel>
#include <QHashData>
#include <QList>
#include <QDebug>
#include "game.h"

class GameListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    GameListModel();
    ~GameListModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int,QByteArray> roleNames() const;

    void addAll(const QList<Game*> &);
    void addGame(Game*);
    void removeGame(Game*);
    Game *find(const uint);
    void clear();

    enum Roles {
        NameRole =          Qt::UserRole + 1,
        LogoRole =          Qt::UserRole + 2,
        PreviewRole =       Qt::UserRole + 3,
        ViewersRole =       Qt::UserRole + 4
    };

private:
    QList<Game*> games;

public slots:
    int count();
};


#endif // GAMELISTMODEL_H
