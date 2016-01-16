#ifndef POWER_H
#define POWER_H

#include <QTimer>
#include <QObject>

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

class Power: public QObject
{
    Q_OBJECT
public:
    Power();
    ~Power();

    Q_INVOKABLE void setScreensaver(bool);

    void setWid(const quint32 &value);

private:
    QTimer* timer;
    quint32 wid;

private slots:
    void onTimerProc();
};

#endif // POWER_H
