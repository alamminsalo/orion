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

#include "notificationmanager.h"
#include <QVariant>
#include <QDebug>

NotificationManager::NotificationManager(QQmlApplicationEngine *engine, QNetworkAccessManager *nm, QObject *parent) :
    QObject(parent),
    net(nm)
{
    currentObject = 0;

    queue.clear();

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &NotificationManager::showNext);
    timer->setInterval(3000);
    timer->setSingleShot(true);

    this->engine = engine;
}

NotificationManager::~NotificationManager()
{
    timer->stop();
    delete timer;

    qDeleteAll(queue);
    queue.clear();
}

void NotificationManager::showNext()
{
    //Pops first in queue, and shows it
    if (currentObject){
        currentObject->deleteLater();
        currentObject = 0;
    }

    if (!queue.isEmpty()){
        NotificationData *data = queue.takeFirst();

#if defined(Q_OS_MAC) || defined (Q_OS_LINUX)
        //NotificationSender deletes itself after displaying message
        NotificationSender *msg = new NotificationSender(net);
        msg->pushNotification(data->title, data->message, data->imgUrl);
#endif
        delete data;

        timer->start();
    }
}

void NotificationManager::pushNotification(const QString &title, const QString &message, const QString &imgUrl)
{
    NotificationData *data = new NotificationData;
    data->title = title;
    data->message = message;
    data->imgUrl = imgUrl;

    queue.append(data);

    if (!timer->isActive()){
        showNext();
    }
}
