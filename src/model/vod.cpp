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

#include "vod.h"

Vod::Vod()
{
    title = "";
    id = "";
    game = "";
    duration = 0;
    views = 0;
    preview = "";
}

Vod::Vod(Vod &other)
{
    title = other.title;
    id = other.id;
    game = other.game;
    views = other.views;
    preview = other.preview;
    duration = other.duration;
}

QString Vod::getPreview() const
{
    return preview;
}

void Vod::setPreview(const QString &value)
{
    preview = value;
}

quint64 Vod::getViews() const
{
    return views;
}

void Vod::setViews(const quint64 &value)
{
    views = value;
}

quint32 Vod::getDuration() const
{
    return duration;
}

void Vod::setDuration(const quint32 &value)
{
    duration = value;
}

QString Vod::getGame() const
{
    return game;
}

void Vod::setGame(const QString &value)
{
    game = value;
}

QString Vod::getId() const
{
    return id;
}

void Vod::setId(const QString &value)
{
    id = value;
}

QString Vod::getTitle() const
{
    return title;
}

void Vod::setTitle(const QString &value)
{
    title = value;
}
