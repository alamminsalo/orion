#include "power.h"
#include <QtGlobal>
#include <QProcess>
#include <QDebug>

Power::Power()
{
    timer = new QTimer();
    wid = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerProc()));
}

Power::~Power()
{
    delete timer;
}

void Power::setScreensaver(bool enabled)
{

#ifdef Q_OS_LINUX
    if (!wid){
        qDebug() << "No window set";
        return;
    }

    qDebug() << "Screensaver change event, wid" << wid;

    QStringList args;

    if (!enabled)
    {
        qDebug() << "DISABLING screensaver";
        args << "suspend" << QString::number(wid);

        QProcess::startDetached("xdg-screensaver reset");

    } else
    {
        qDebug() << "ENABLING screensaver";
        args << "resume" << QString::number(wid);
    }

    QProcess::startDetached("xdg-screensaver",args);

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

void Power::setWid(const quint32 &value)
{
    //qDebug() << "Setting wid for power-saver:" << value;
    wid = value;
}

void Power::onTimerProc()
{
    qDebug() << "Screensaver: Reset event";
    QProcess::startDetached("xdg-screensaver reset");
}
