#include "notificationsender.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtDBus/QtDBus>
#include <QImage>

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


/**
 * Show a notification using the org.freedesktop.Notification service
 * @param summary Summary, should be a short string to summarize the
 *  notification
 * @param text The text of the notification
 * @param image Additional image to show among the notification
 *
 * Copyright (C) 2012 Roland Hieber <rohieb@rohieb.name>
 * LICENSE GPL3
 *
 */
void NotificationSender::sendNotification(const QString &title, const QString &message, const QByteArray &data)
{
    QVariantMap hints;
    hints["image_data"] = QImage::fromData(data);

    QList<QVariant> argumentList;
    argumentList << APP_NAME; //app_name
    argumentList << (uint)0;  // replace_id
    argumentList << "";  // app_icon
    argumentList << title; // summary
    argumentList << message; // body
    argumentList << QStringList();  // actions
    argumentList << hints;  // hints
    argumentList << (int)5000; // timeout in ms

    QDBusInterface dbus_notify("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
    QDBusMessage reply = dbus_notify.callWithArgumentList(QDBus::AutoDetect, "Notify", argumentList);

    if(reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "D-Bus Error:" << reply.errorMessage();
    }
}

/**
 * Automatic marshaling of a QImage for org.freedesktop.Notifications.Notify
 *
 * This function is from the Clementine project (see
 * http://www.clementine-player.org) and licensed under the GNU General Public
 * License, version 3 or later.
 *
 * Copyright 2010, David Sansome <me@davidsansome.com>
 */
QDBusArgument& operator<<(QDBusArgument& arg, const QImage& image) {
    if (image.isNull()) {
            // Sometimes this gets called with a null QImage for no obvious reason.
            arg.beginStructure();
            arg << 0 << 0 << 0 << false << 0 << 0 << QByteArray();
            arg.endStructure();
            return arg;
        }
        QImage scaled = image.scaledToHeight(128, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);

        #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        // ABGR -> ARGB
        QImage i = scaled.rgbSwapped();
        #else
        // ABGR -> GBAR
        QImage i(scaled.size(), scaled.format());
        for (int y = 0; y < i.height(); ++y) {
            QRgb *p = (QRgb*) scaled.scanLine(y);
            QRgb *q = (QRgb*) i.scanLine(y);
            QRgb *end = p + scaled.width();
            while (p < end) {
                *q = qRgba(qGreen(*p), qBlue(*p), qAlpha(*p), qRed(*p));
                p++;
                q++;
            }
        }
        #endif

        arg.beginStructure();
        arg << i.width();
        arg << i.height();
        arg << i.bytesPerLine();
        arg << i.hasAlphaChannel();
        int channels = i.isGrayscale() ? 1 : (i.hasAlphaChannel() ? 4 : 3);
        arg << i.depth() / channels;
        arg << channels;
        arg << QByteArray(reinterpret_cast<const char*>(i.bits()), i.byteCount());
        arg.endStructure();
        return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, QImage&) {
  // This is needed to link but shouldn't be called.
  Q_ASSERT(0);
  return arg;
}
