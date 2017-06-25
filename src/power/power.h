/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef POWER_H
#define POWER_H

#include <QObject>
#include "../model/singletonprovider.h"

#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

class Power: public QObject
{
    QML_SINGLETON
    Q_OBJECT

    Q_PROPERTY(bool screensaver WRITE setScreensaver)

    Power();

    static Power *instance;

public:
    static Power *getInstance();
    ~Power();

    Q_INVOKABLE void setScreensaver(bool);

private:
    quint32 cookie;

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event);
};

#endif // POWER_H
