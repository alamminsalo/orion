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

#ifndef VOD_H
#define VOD_H

#include <QString>
#include <QObject>

class Vod
{
    QString title = "";
    QString id = "";
    QString game = "";
    quint32 duration = 0;
    quint64 views = 0;
    QString preview = "";
    QString createdAt = "";
    QString seekPreviews = "";

public:
    Vod();
    Vod(Vod &other) = default;
    ~Vod(){};

    QString getPreview() const;
    void setPreview(const QString &value);
    quint64 getViews() const;
    void setViews(const quint64 &value);
    quint32 getDuration() const;
    void setDuration(const quint32 &value);
    QString getGame() const;
    void setGame(const QString &value);
    QString getId() const;
    void setId(const QString &value);
    QString getTitle() const;
    void setTitle(const QString &value);
    QString getCreatedAt() const;
    void setCreatedAt(const QString &value);
    QString getSeekPreviews() const;
    void setSeekPreviews(const QString &value);
};

#endif // VOD_H
