#ifndef POWER_H
#define POWER_H

#include <QTimer>
#include <QObject>
#include <QApplication>

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

class Power: public QObject
{
    Q_OBJECT
public:
    Power(QApplication *app);
    ~Power();

    Q_INVOKABLE void setScreensaver(bool);

private:
    QTimer* timer;
    QApplication *app;
    quint32 cookie;

private slots:
    void onTimerProc();
};

#endif // POWER_H
