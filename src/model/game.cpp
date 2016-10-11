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

#include "game.h"


QString Game::getName() const
{
    return name;
}

void Game::setName(const QString &value)
{
    name = value;
}

QString Game::getLogo() const
{
    return logo;
}

void Game::setLogo(const QString &value)
{
    logo = value;
}

quint32 Game::getViewers() const
{
    return viewers;
}

void Game::setViewers(const quint32 &value)
{
    viewers = value;
}

QString Game::getPreview() const
{
    return preview;
}

void Game::setPreview(const QString &value)
{
    preview = value;
}
uint Game::getId() const
{
    return id;
}

void Game::setId(const uint &value)
{
    id = value;
}

Game::Game(){
    viewers = 0;
}

//Copy constructor
Game::Game(const Game &other)
{
    setViewers(other.getViewers());
    setName(other.getName());
    setLogo(other.getLogo());
    setPreview(other.getPreview());
}
