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
