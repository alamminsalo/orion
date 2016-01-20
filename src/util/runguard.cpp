#include "runguard.h"

#include <QDebug>
#include <QCryptographicHash>
#include <QObject>

namespace
{

QString generateKeyHash( const QString& key, const QString& salt )
{
    QByteArray data;

    data.append( key.toUtf8() );
    data.append( salt.toUtf8() );
    data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

    return data;
}

}


RunGuard::RunGuard( const QString& key )
    : key( key )
    , memLockKey( generateKeyHash( key, "_memLockKey" ) )
    , sharedmemKey( generateKeyHash( key, "_sharedmemKey" ) )
    , sharedMem( sharedmemKey )
    , memLock( memLockKey, 1 )
{
    memLock.acquire();
    {
        QSharedMemory fix( sharedmemKey );    // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }
    memLock.release();
}

RunGuard::~RunGuard()
{
    timer->stop();
    timer->deleteLater();
    release();
}

bool RunGuard::isAnotherRunning()
{
    if ( sharedMem.isAttached() ){
        return false;
    }

    memLock.acquire();
    const bool isRunning = sharedMem.attach();
    if ( isRunning ){
        sharedMem.detach();   
    }
    memLock.release();

    return isRunning;
}

bool RunGuard::tryToRun()
{
    if ( isAnotherRunning() ){   // Extra check
        return false;
    }

    memLock.acquire();
    const bool result = sharedMem.create( sizeof( quint64 ) );
    memLock.release();
    if ( !result )
    {
        release();
        return false;
    }

    setTimer();

    return true;
}

void RunGuard::release()
{
    memLock.acquire();
    if ( sharedMem.isAttached() )
        sharedMem.detach();
    memLock.release();
}

void RunGuard::setTimer()
{
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(200);
}

void RunGuard::sendWakeup()
{
    sharedMem.attach();
    sharedMem.lock();
    *(quint64*)sharedMem.data() = 1;
    sharedMem.unlock();
    sharedMem.detach();
}

void RunGuard::update()
{
    if (*(quint64*)sharedMem.constData() == 1){
        qDebug() << "Another process attempted to start!";

        //Reset back the flag
        sharedMem.lock();
        *(quint64*)sharedMem.data() = 0;
        sharedMem.unlock();

        emit anotherProcessTriggered();
    }
}











