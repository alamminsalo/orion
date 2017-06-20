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

#include "systray.h"

#include <QAction>

SysTray::SysTray(QObject *parent):
    QSystemTrayIcon(parent)
{
    menu = new QMenu();

    QAction *show = new QAction(menu);
    show->setText("Show/Hide");
    connect(show, &QAction::triggered, this, &SysTray::showSlot);
    menu->addAction(show);

    QAction *quit = new QAction(menu);
    quit->setText("Quit");
    connect(quit, &QAction::triggered, this, &SysTray::quitSlot);
    menu->addAction(quit);

    connect(this, &SysTray::activated, this, &SysTray::clickSlot);

    setContextMenu(menu);
}

SysTray::~SysTray()
{
    delete menu;
}

void SysTray::quitSlot()
{
    emit closeEventTriggered();
}

void SysTray::clickSlot(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger){
//Dont trigger this on mac
#ifndef Q_OS_MAC
        emit showTriggered();
#endif
    }
}

void SysTray::showSlot()
{
    emit showTriggered();
}
