#ifndef SNORENOTIF_H
#define SNORENOTIF_H



#include <libsnore/snore_exports.h>
#include <libsnore/snore.h>

#include <QString>
#include <QThread>
#include <QUrl>

using namespace Snore;

class Q_DECL_EXPORT SnoreNotif: public QObject
{
    Q_OBJECT

public:
    SnoreNotif();

    Application app;
    void notify(const QString &title, const QString &text, const QString &url);
};

class NotificationRunner : public QThread
{
    Q_OBJECT
public:
    NotificationRunner(const Application &app, const QString &title, const QString &text, const QString &url);

private:
    Application app;
    QString title;
    QString text;
    QString url;

    virtual void run(){
        Notification n(app, Alert(), title, text, Icon::fromWebUrl(QUrl(url)));
        SnoreCore::instance().broadcastNotification(n);
        this->deleteLater();
    }
};

#endif // SNORENOTIF_H
