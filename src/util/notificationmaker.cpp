#include "notificationmaker.h"
#include <QVariant>
#include <QDebug>

NotificationMaker::NotificationMaker(QQmlApplicationEngine *engine)
{
    currentObject = 0;

    queue.clear();

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(showNext()));
    timer->setInterval(4000);
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

NotificationMaker::~NotificationMaker()
{
    timer->stop();
    delete timer;

#ifdef Q_OS_WIN
    hiddenWindow->close();
    delete hiddenWindow;
#endif

    qDeleteAll(queue);
    queue.clear();
}

void NotificationMaker::showNext()
{
    //Pops first in queue, and shows it
    if (currentObject){
        currentObject->deleteLater();
        currentObject = 0;
    }

    if (!queue.isEmpty()){

        NotificationData *data = queue.takeFirst();

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

        delete data;

        timer->start();
    }
}

void NotificationMaker::pushNotification(const QString title, const QString message, const QString imgUrl)
{
    qDebug() << "Notificator: Received notification: " << title << ", " << message << ", " << imgUrl;
    NotificationData *data = new NotificationData;
    data->title = title;
    data->message = message;
    data->imgUrl = imgUrl;

    queue.append(data);

    if (!timer->isActive()){
        showNext();
    }
}
