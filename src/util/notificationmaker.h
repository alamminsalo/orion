#ifndef NOTIFICATIONMAKER_H
#define NOTIFICATIONMAKER_H

#include <QTimer>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QObject>

typedef struct {
    QString title;
    QString message;
    QString imgUrl;
} NotificationData;

class NotificationMaker: public QObject
{
    Q_OBJECT
public:
    NotificationMaker(QQmlApplicationEngine *engine);
    virtual ~NotificationMaker();

private slots:
    void showNext();

public slots:
    void pushNotification(const QString title, const QString message, const QString imgUrl);

private:
    QQmlApplicationEngine *engine;
    QTimer *timer;
    QList<NotificationData*> queue;
    QObject *currentObject;
};

#endif // NOTIFICATIONMAKER_H
