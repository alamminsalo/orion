#include "power.h"
#include <QtGlobal>
#include <QProcess>
#include <QDebug>
#include <QWindow>

#ifdef Q_OS_LINUX
    #include <QtDBus>
#endif

Power::Power(QApplication *app) :
    app(app), cookie(0)
{
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerProc()));
}

Power::~Power()
{
    setScreensaver(true);
    delete timer;
}

void Power::setScreensaver(bool enabled)
{

#ifdef Q_OS_LINUX

    if (!enabled) {
        if (cookie == 0) {
            QDBusInterface dbus("org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver", "org.freedesktop.ScreenSaver");
            QDBusMessage reply = dbus.call("Inhibit", APP_NAME, "Playing video");

            if(reply.type() != QDBusMessage::ErrorMessage) {
                cookie = reply.arguments().at(0).toInt();
                qDebug() << "Disabled screensaver with cookie " << cookie;
            }
        }
    } else {
        if (cookie > 0) {
            QDBusInterface dbus("org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver", "org.freedesktop.ScreenSaver");
            QDBusMessage reply = dbus.call("UnInhibit", QVariant(cookie));

            if(reply.type() != QDBusMessage::ErrorMessage) {
                cookie = 0;
                qDebug() << "Enabled screensaver";
            }
        }
    }

//    QWindow *win = app->allWindows().at(0);

//    if (!win)
//        return;

//    quint32 wid = win->winId();

//    qDebug() << "Screensaver change event, wid" << wid;

//    QStringList args;
//    args << (enabled ? "resume" : "suspend") << QString::number(wid);

//    QProcess::startDetached("xdg-screensaver",args);

//    // Also set timer to send nudges to xserver
//    if (enabled){
//        timer->stop();
//    } else {
//        if (!timer->isActive()){
//            timer->start(25000);
//        }
//    }

#elif defined(Q_OS_WIN)
    if (!enabled)
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
    else
        SetThreadExecutionState(ES_CONTINUOUS);

#endif
}

void Power::onTimerProc()
{
    qDebug() << "Screensaver: Reset event";
    QProcess::startDetached("xdg-screensaver reset");
}
