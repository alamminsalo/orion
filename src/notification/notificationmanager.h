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

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QObject>

#ifdef Q_OS_WIN
    #include <QQuickWindow>
#endif

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    #include "notificationsender.h"

#endif

typedef struct {
    QString title;
    QString message;
    QString imgUrl;
} NotificationData;

class NotificationManager: public QObject
{
    Q_OBJECT
public:
    NotificationManager(QQmlApplicationEngine *engine, QNetworkAccessManager *nm);
    virtual ~NotificationManager();

private slots:
    void showNext();

public slots:
    void pushNotification(const QString &title, const QString &message, const QString &imgUrl);

private:
    QQmlApplicationEngine *engine;

    QNetworkAccessManager *net;

    QTimer *timer;

    QList<NotificationData*> queue;

    QObject *currentObject;

#ifdef Q_OS_WIN
    QQuickWindow *hiddenWindow;
#endif
};

#endif // NOTIFICATIONMANAGER_H
