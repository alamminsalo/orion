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

#include "notificationsender.h"
#include <QNetworkReply>
#include <QNetworkRequest>

#ifdef Q_OS_MAC
#import <NSUserNotification.h>
#endif

NotificationSender::NotificationSender(QNetworkAccessManager *nm) : netman(nm)
{
#ifdef Q_OS_LINUX
    qDBusRegisterMetaType<QImage>();

#endif
}

NotificationSender::~NotificationSender()
{
}

void NotificationSender::pushNotification(const QString &title, const QString &subtitle, const QString &url)
{
    this->title = title;
    this->subtitle = subtitle;

    if (!url.isEmpty())
        getFile(url);
    else
        pushNotification(this->title, this->subtitle);
}

void NotificationSender::getFile(const QString &url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *reply = netman->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(onFileReply()));
}


void NotificationSender::onFileReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    sendNotification(title, subtitle, reply->readAll());

    reply->deleteLater();
}

#ifdef Q_OS_MAC

void NotificationSender::sendNotification(const QString &title, const QString &message, const QByteArray &data)
{
    NSUserNotification *notification = [[[NSUserNotification alloc] init] autorelease];

    notification.title = (NSString *) title.toNSString();
    notification.informativeText = (NSString *) message.toNSString();

    if (!data.isEmpty()) {
        NSImage *image = [[[NSImage alloc] initWithData:(NSData *)data.toNSData()] autorelease];
        [notification setValue:image forKey:@"_identityImage"];
    }

//    [notification setValue:YES forKey:@"_showsButtons"];
//    notification.actionButtonTitle = @"Watch";

    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];

    this->deleteLater();
}

#endif


