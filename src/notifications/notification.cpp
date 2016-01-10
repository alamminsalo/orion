#ifdef ENABLE_NOTIFY
#include "notification.h"

#define DEFAULT_LOGO "resources/logos/default.png"

SnoreNotif::SnoreNotif() :
    app(QStringLiteral(""), QIcon(DEFAULT_LOGO))
{
    SnoreCore &instance = SnoreCore::instance();
    instance.loadPlugins(SnorePlugin::Backend);
    instance.setSettingsValue(QStringLiteral("Timeout"), 5, LocalSetting);
    instance.registerApplication(app);
}

void SnoreNotif::notify(const QString &title, const QString &text, const QString &url)
{
    NotificationRunner* runner = new NotificationRunner(app, title, text, url);
    runner->start();
}

NotificationRunner::NotificationRunner(const Application &app, const QString &title, const QString &text, const QString &url):
    app(app),title(title),text(text),url(url)
{

}

#endif
