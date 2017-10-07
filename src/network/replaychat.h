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

#ifndef REPLAYCHAT_H
#define REPLAYCHAT_H

struct ReplayChatMessage {
    QString id;
    QString from;
    bool deleted;
    QString message;
    QString room;
    QString command;
    double timestamp;
    double videoOffset;
    QVariantMap tags;
    QMap<int, QPair<int, int>> emotePositionsMap; // first -> (last, emoteId)
    QList<int> emoteList;
};

struct ReplayChatPiece {
    QList<ReplayChatMessage> comments;
    QString next;
    QString prev;
};

#endif
