#include "mainwindow.h"
#include "traywindow.h"
#include <QApplication>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow main;
    main.show();

    TrayWindow w(&main);
    w.show();

    return a.exec();
}
