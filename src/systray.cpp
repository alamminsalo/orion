#include "systray.h"

#include <QAction>

SysTray::SysTray()
{
    menu = new QMenu();

    QAction *show = new QAction(menu);
    show->setText("Show/Hide");
    connect(show, SIGNAL(triggered(bool)), this, SLOT(showSlot()));
    menu->addAction(show);

    QAction *quit = new QAction(menu);
    quit->setText("Quit");
    connect(quit, SIGNAL(triggered(bool)), this, SLOT(quitSlot()));
    menu->addAction(quit);

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(clickSlot(QSystemTrayIcon::ActivationReason)));

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
