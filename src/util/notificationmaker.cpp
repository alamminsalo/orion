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
}

NotificationMaker::~NotificationMaker()
{
    timer->stop();
    delete timer;

    qDeleteAll(queue);
    queue.clear();
}

void NotificationMaker::showNext()
{
    //Pops first in queue, and shows it
    if (currentObject){
        delete currentObject;
        currentObject = 0;
    }

    if (!queue.isEmpty()){

        NotificationData *data = queue.takeFirst();

        QQmlComponent component(engine, QUrl("qrc:/components/Notification.qml"));

        // Show notification
        currentObject = component.create();

        // Set data to notification
        currentObject->setProperty("title", QVariant::fromValue(data->title));
        currentObject->setProperty("description", QVariant::fromValue(data->message));
        currentObject->setProperty("imgSrc", QVariant::fromValue(data->imgUrl));

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
