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
