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

#ifdef Q_OS_WIN //Workaround to shitty win(10?) windowing system, which hides ALL windows on minimize/ctr, including popups
    hiddenWindow = new QQuickWindow();
    hiddenWindow->setFlags(Qt::FramelessWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    hiddenWindow->setPosition(QPoint(0,0));
    hiddenWindow->setMinimumSize(QSize(0,0));
    hiddenWindow->setMaximumSize(QSize(1,1));
    hiddenWindow->setHeight(1);
    hiddenWindow->setOpacity(0);
    hiddenWindow->setWidth(1);
    hiddenWindow->show();
#endif
}

NotificationManager::~NotificationManager()
{
    timer->stop();
    delete timer;

#ifdef Q_OS_WIN
    hiddenWindow->close();
    delete hiddenWindow;
#endif

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

#ifdef Q_OS_WIN
        QQmlComponent component(engine, QUrl("qrc:/components/Notification.qml"));
        component.setParent(0);

        // Show notification
        currentObject = component.create(engine->rootContext());

        // Set data to notification
        currentObject->setProperty("title", QVariant::fromValue(data->title));
        currentObject->setProperty("description", QVariant::fromValue(data->message));
        currentObject->setProperty("imgSrc", QVariant::fromValue(data->imgUrl));

        QObject::connect(currentObject, SIGNAL(clicked()), this, SLOT(showNext()));

        // Trigger notification
        currentObject->setProperty("visible", QVariant::fromValue(true));
#endif

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
