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

#ifndef VODMANAGER_H
#define VODMANAGER_H

#include <QObject>
#include "../network/networkmanager.h"
#include "vodlistmodel.h"

class VodManager: public QObject
{
    Q_OBJECT

public:
    VodManager(NetworkManager *netman);
    ~VodManager();

    Q_INVOKABLE void search(const QString channelName, const quint32 offset, const quint32 limit);

    Q_INVOKABLE QString getGame() const;

    Q_INVOKABLE void getBroadcasts(QString vod);

    VodListModel *getModel() const;

public slots:
    void onSearchFinished(QList<Vod *>);
    void onStreamGetFinished(QStringList);

signals:
    void searchStarted();
    void searchFinished();
    void streamsGetFinished(QStringList items);

private:
    QString game;
    VodListModel *model;
    NetworkManager *netman;
};

#endif // VODMANAGER_H
