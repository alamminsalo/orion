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

#ifndef NOTIFICATIONSENDER_H
#define NOTIFICATIONSENDER_H

/**
  * NotificationSender
  *
  * Sends a notification to system desktop
  * Make sure to implement delete on pushNotification
  * */

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

class NotificationSender: public QObject
{
    Q_OBJECT

    void getFile(const QString &url);
    void sendNotification(const QString &title, const QString &subtitle, const QByteArray &data = QByteArray());

public:

    NotificationSender(QNetworkAccessManager *nm);
    ~NotificationSender();

    void pushNotification(const QString &title, const QString &message, const QString &url = QString());

private slots:
    void onFileReply();

private:
    QNetworkAccessManager *netman;

    QString title;
    QString subtitle;
};

#endif // NOTIFICATIONSENDER_H
