#include "traywindow.h"
#include <iostream>

TrayWindow::TrayWindow(MainWindow *w)
{
    this->w = w;
    this->setIcon(QIcon("./test.jpg"));
    this->setupMenu();
    this->setContextMenu(&qmenu);
    QObject::connect(this,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(activated(QSystemTrayIcon::ActivationReason)));
}

void TrayWindow::setupMenu(){
    qmenu.addAction("Channel list");
    qmenu.addSeparator();
    connect(qmenu.actions().at(0),SIGNAL(triggered()),this,SLOT(showMainWindow()));
}

void TrayWindow::activated(QSystemTrayIcon::ActivationReason rs){
    if (rs == 3){
        this->showMainWindow();
    }
}

void TrayWindow::showMainWindow(){
    w->show();
}
