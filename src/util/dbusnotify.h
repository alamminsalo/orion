#ifndef DBUSNOTIFY_H
#define DBUSNOTIFY_H

#ifdef HAVE_DBUS

#include <QtDBus>
#include <QImage>

    const QString APPNAME("Stratumsphere Tray Icon");
    const QString VERSION("0.9375");

    /**
     * Show a notification using the org.freedesktop.Notification service
     * @param summary Summary, should be a short string to summarize the
     *  notification
     * @param text The text of the notification
     * @param image Additional image to show among the notification
     */
    bool showNotification(const QString summary,
      const QString text, const QImage image = QImage()) {

      qDBusRegisterMetaType<QImage>();

      QVariantMap hints;
      hints["image_data"] = image;
      QList<QVariant> argumentList;
      argumentList << APPNAME; //app_name
      argumentList << (uint)0;  // replace_id
      argumentList << "";  // app_icon
      argumentList << summary; // summary
      argumentList << text; // body
      argumentList << QStringList();  // actions
      argumentList << hints;  // hints
      argumentList << (int)5000; // timeout in ms

      static QDBusInterface notifyApp("org.freedesktop.Notifications",
        "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
      QDBusMessage reply = notifyApp.callWithArgumentList(QDBus::AutoDetect,
        "Notify", argumentList);
      if(reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "D-Bus Error:" << reply.errorMessage();
        return false;
      }
      return true;
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
      if(image.isNull()) {
        arg.beginStructure();
        arg << 0 << 0 << 0 << false << 0 << 0 << QByteArray();
        arg.endStructure();
        return arg;
      }

      QImage scaled = image.scaledToHeight(100, Qt::SmoothTransformation);
      scaled = scaled.convertToFormat(QImage::Format_ARGB32);

    #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
      // ABGR -> ARGB
      QImage i = scaled.rgbSwapped();
    #else
      // ABGR -> GBAR
      QImage i(scaled.size(), scaled.format());
      for (int y = 0; y < i.height(); ++y) {
        QRgb* p = (QRgb*) scaled.scanLine(y);
        QRgb* q = (QRgb*) i.scanLine(y);
        QRgb* end = p + scaled.width();
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

#endif // def HAVE_DBUS

#endif // DBUSNOTIFY_H
