#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QObject>
#include <QTimer>
#include <QSharedMemory>
#include <QSystemSemaphore>


class RunGuard: public QObject
{
    Q_OBJECT
public:
    RunGuard( const QString& key );
    virtual ~RunGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();
    void setTimer();
    void sendWakeup();

public slots:
    void update();

signals:
    void anotherProcessTriggered();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QTimer *timer;
    QSharedMemory sharedMem;
    QSystemSemaphore memLock;
};


#endif // RUNGUARD./
