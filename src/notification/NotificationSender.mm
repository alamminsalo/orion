#include "notificationsender.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#import <NSUserNotification.h>

NotificationSender::NotificationSender(QNetworkAccessManager *nm) : netman(nm)
{
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

    QByteArray data = reply->readAll();

    sendNotification(title, subtitle, data);

    reply->deleteLater();
}

#ifdef Q_OS_MAC

void NotificationSender::sendNotification(const QString &title, const QString &subtitle, const QByteArray &data)
{
    NSUserNotification *notification = [[[NSUserNotification alloc] init] autorelease];

    notification.title = (NSString *) title.toNSString();
    notification.informativeText = (NSString *) subtitle.toNSString();

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


