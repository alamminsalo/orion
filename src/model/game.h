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

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QString>

class Game : public QObject
{
    Q_OBJECT

    uint id;
    QString name;
    QString logo;
    QString preview;
    quint32 viewers;

public:
    Game();
    Game(const Game&);
    ~Game(){}

    QString getName() const;
    void setName(const QString &value);

    QString getLogo() const;
    void setLogo(const QString &value);

    quint32 getViewers() const;
    void setViewers(const quint32 &value);

    QString getPreview() const;
    void setPreview(const QString &value);

    uint getId() const;
    void setId(const uint &value);

signals:
    void updated();
};
Q_DECLARE_METATYPE(Game)

#endif // GAME_H
