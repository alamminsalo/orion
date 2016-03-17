#include "notificationmanager.h"
#include <QVariant>
#include <QDebug>

#ifdef Q_OS_MAC
#include "notificationsender.h"

#endif

NotificationManager::NotificationManager(QQmlApplicationEngine *engine)
{
    net = new QNetworkAccessManager();
    net->connectToHost("http://static-cdn.jtvnw.net");
    currentObject = 0;

    queue.clear();

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(showNext()));
    timer->setInterval(5000);
    timer->setSingleShot(true);

    this->engine = engine;

#ifdef Q_OS_WIN //Workaround to shitty win(10?) windowing system, which hides ALL windows on minimize/ctr, including popups
    hiddenWindow = new QQuickWindow();
    hiddenWindow->setFlags(Qt::FramelessWindowHint | Qt::BypassWindowManagerHint | Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    hiddenWindow->setPosition(QPoint(0,0));
    hiddenWindow->setMinimumSize(QSize(0,0));
    hiddenWindow->setMaximumSize(QSize(1,1));
    hiddenWindow->setHeight(1);
    hiddenWindow->setOpacity(0);
    hiddenWindow->setWidth(1);
    hiddenWindow->show();
#endif
}

NotificationManager::~NotificationManager()
{
    timer->stop();
    delete timer;

    net->deleteLater();

#ifdef Q_OS_WIN
    hiddenWindow->close();
    delete hiddenWindow;
#endif

    qDeleteAll(queue);
    queue.clear();
}

void NotificationManager::showNext()
{
    //Pops first in queue, and shows it
    if (currentObject){
        currentObject->deleteLater();
        currentObject = 0;
    }

    if (!queue.isEmpty()){

        NotificationData *data = queue.takeFirst();

#ifdef Q_OS_WIN
        QQmlComponent component(engine, QUrl("qrc:/components/Notification.qml"));
        component.setParent(0);

        // Show notification
        currentObject = component.create(engine->rootContext());

        // Set data to notification
        currentObject->setProperty("title", QVariant::fromValue(data->title));
        currentObject->setProperty("description", QVariant::fromValue(data->message));
        currentObject->setProperty("imgSrc", QVariant::fromValue(data->imgUrl));

        QObject::connect(currentObject, SIGNAL(clicked()), this, SLOT(showNext()));

        // Trigger notification
        currentObject->setProperty("visible", QVariant::fromValue(true));
#endif

#ifdef Q_OS_MAC
        //NotificationSender deletes itself after displaying message
        NotificationSender *msg = new NotificationSender(net);
        msg->pushNotification(data->title, data->message, data->imgUrl);
#endif
        delete data;

        timer->start();
    }
}

void NotificationManager::pushNotification(const QString &title, const QString &message, const QString &imgUrl)
{
    NotificationData *data = new NotificationData;
    data->title = title;
    data->message = message;
    data->imgUrl = imgUrl;

    queue.append(data);

    if (!timer->isActive()){
        showNext();
    }
}
