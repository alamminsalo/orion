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
