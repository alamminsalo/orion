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

#include "gamelistmodel.h"

GameListModel::GameListModel()
{
}

GameListModel::~GameListModel()
{
    qDebug() << "Destroyer: GamesModel";
    clear();
}

Qt::ItemFlags GameListModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled;
}

QVariant GameListModel::data(const QModelIndex &index, int role) const
{
    QVariant var;

    if (!index.isValid()){
        return var;
    }

    Game *game = games.at(index.row());

    if (game){
        switch(role){
            case NameRole:
                var.setValue(game->getName());
                break;

            case LogoRole:
                var.setValue(game->getLogo());
                break;

            case PreviewRole:
                var.setValue(game->getPreview());
                break;

            case ViewersRole:
                var.setValue(game->getViewers());
                break;
        }
    }

    return var;
}

int GameListModel::rowCount(const QModelIndex &parent) const
{
    return games.size();
}

QHash<int, QByteArray> GameListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[LogoRole] = "logo";
    roles[PreviewRole] = "preview";
    roles[ViewersRole] = "viewers";
    return roles;
}

void GameListModel::addAll(const QList<Game *> &list)
{
    if (!list.isEmpty()){
        emit beginInsertRows(QModelIndex(), games.size(), games.size() + list.size() - 1);
        foreach (Game *game, list) {
            games.append(new Game(*game));
        }
        emit endInsertRows();
    }
}

void GameListModel::addGame(Game *game)
{
    emit beginInsertRows(QModelIndex(), games.size(), games.size());
    games.append(game);
    emit endInsertRows();
}

void GameListModel::removeGame(Game *game)
{
    int index = games.indexOf(game);
    if (index > -1){
        emit beginRemoveRows(QModelIndex(), index, index);
        delete games.takeAt(index);
        emit endRemoveRows();
    }
}

Game *GameListModel::find(const uint id)
{
    foreach(Game *game, games){
        if (game->getId() == id){
            return game;
        }
    }
    return 0;
}

void GameListModel::clear()
{
    if (!games.isEmpty()){
        emit beginRemoveRows(QModelIndex(), 0, games.size());
        qDeleteAll(games);
        games.clear();
        emit endRemoveRows();
    }
}

int GameListModel::count()
{
    return rowCount();
}
