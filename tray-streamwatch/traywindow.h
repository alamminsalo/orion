#ifndef TRAYWINDOW_H
#define TRAYWINDOW_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QDebug>
#include "mainwindow.h"

class TrayWindow: public QSystemTrayIcon
{
    Q_OBJECT
private:
    MainWindow *w;
    QMenu qmenu;

    void setupMenu();

public:
    TrayWindow(MainWindow*);

protected slots:
    void activated(QSystemTrayIcon::ActivationReason);
    void showMainWindow();

};

#endif // TRAYWINDOW_H
