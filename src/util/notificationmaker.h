#ifndef NOTIFICATIONMAKER_H
#define NOTIFICATIONMAKER_H

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

#ifdef Q_OS_WIN
    QQuickWindow *hiddenWindow;
#endif
};

#endif // NOTIFICATIONMAKER_H
